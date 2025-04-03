# 폴링 간격과 지연/CPU 트레이드오프

**Lv2-core / 동시성/성능**

**프로젝트에서** `src/time.c:36-60`의 `philo_sleep_ms()`와 `src/monitor.c:30-65`의 `philo_monitor()` 둘 다, 목표 시간까지 한 번에 자지 않고 짧은 간격으로 나눠 자면서 매번 종료 조건을 재확인한다.

```c
// src/time.c:53-58
remaining = deadline - now;
if (remaining > 1)
	  usleep(500);
else
	  usleep(100);
```

```c
// src/monitor.c:64
usleep(500);
```

**일반적으로** 폴링(polling)은 "조건이 바뀌었는지 주기적으로 확인한다"는 방식이고, 그 반대편에는 "조건이 바뀌는 순간 알림을 받는다"는 이벤트 기반(예: 조건변수, epoll, 인터럽트)이 있다. 폴링 간격을 정하는 건 명확한 트레이드오프다. 간격을 촘촘히 하면(짧게 자면) 조건 변화를 더 빨리 감지하지만 그만큼 CPU를 자주 깨워 쓰고, 간격을 넓히면(길게 자면) CPU는 아끼지만 실제 변화와 감지 사이의 지연이 커진다. 이 트레이드오프는 폴링을 쓰는 모든 시스템(헬스체크, 배치 작업 스케줄러, 파일 변경 감시 등)에 공통으로 적용된다.

**모범사례**:  
이 프로젝트는 폴링 대신 이벤트 기반(예: 사망 조건을 감시하는 전용 조건변수)을 택하지 않았다. 왜냐하면 "아직 죽지 않았다"는 조건은 어떤 스레드가 능동적으로 "지금 누가 죽었다"고 알려주는 이벤트가 아니라, 그저 시간이 흘러서 자연히 성립하는 조건(데드라인 경과)이기 때문이다. 이런 "시간 기반 조건"은 애초에 이벤트로 표현하기 어렵고, 결국 어떤 형태로든 주기적으로 시계를 확인하는 코드가 필요하다. 다만 이 프로젝트는 그 폴링 간격을 고정값 하나로 두지 않고, 데드라인이 임박하면(`remaining <= 1`) 100us로 좁혀서 "평소엔 CPU를 아끼되, 정확도가 중요한 순간에는 정밀하게" 절충했다. 이게 단일 고정 간격보다 나은 점이다.

500us와 100us라는 수치는 정밀 측정이 아니라 실용적 경험치다. 42 과제의 채점 스크립트는 보통 `time_to_die`를 60~800ms 범위로 설정하는데, 이 스케일에서 500us(0.5ms)의 오차는 무시할 만한 수준이면서도 `usleep`을 초당 수백~수천 번 부르는 것보다는 훨씬 CPU를 아낀다. 반대로 데드라인이 1ms 이내로 임박했을 때는 500us 간격 그대로 두면 최악의 경우 그 절반(250us 평균, 최대 500us)만큼 판정이 늦어질 수 있어 100us로 좁혔다. 이 경계값(`remaining > 1`)과 두 상수(500/100) 자체는 실측 대신 "합리적으로 보이는 수준"으로 고정한 값이라, `REPORT.md`도 이를 "실용적 절충값"이라고 명시하며 정밀한 벤치마크 근거는 없다고 스스로 밝히고 있다.

## 웹에서는 어디에 나타나는가

실서비스에서는 폴링 간격을 고정하지 않고, 최근 변화 빈도에 따라 늘렸다 줄였다 하는 적응형(adaptive) 폴링을 쓰기도 한다(예: exponential backoff로 변화가 없으면 점점 간격을 늘리다가, 변화가 감지되면 다시 좁히는 방식). 더 나아가 조건 자체가 "외부 이벤트"로 표현 가능한 경우(파일 변경, 소켓 데이터 도착, DB 트리거)라면 폴링 자체를 없애고 `inotify`, `epoll`, DB의 `LISTEN/NOTIFY` 같은 커널/DB 차원의 알림 메커니즘으로 대체하는 것이 CPU와 지연 양쪽에서 더 낫다. 이 프로젝트가 이런 대안을 쓰지 않은 이유는 앞서 말했듯 "시간이 흐르는 것" 자체를 이벤트로 관찰할 표준적인 방법이 없기 때문이다. 타이머 기반 조건은 결국 어느 시스템에서든 최소한의 폴링(또는 커널의 타이머 인터럽트에 위임하는 `timerfd` 류의 장치)으로 귀결된다.

Node.js에서는 이 구분이 이벤트 루프 차원에서 그대로 드러난다. 조건 변화를 외부에서 알려주는 경우에는 이벤트 리스너나 `LISTEN/NOTIFY`로 폴링을 없애고, 그렇지 않은 경우에만 폴링을 쓰되 간격을 조절한다.

```ts
// ❌ 고정 간격 폴링: 변화가 없어도 매번 DB를 조회하고, 간격만큼 감지가 늦어진다
async function waitForJob(id: string) {
  while (true) {
    const { status } = await db.one('SELECT status FROM jobs WHERE id = $1', [id]);
    if (status === 'done') return;
    await new Promise((r) => setTimeout(r, 100));
  }
}
```

폴링을 유지해야 한다면 변화가 없는 동안 간격을 점점 늘려 부하를 줄인다. C 코드가 데드라인 임박 시 100us로 좁히는 것과 방향만 반대일 뿐, 상황에 따라 간격을 조절한다는 원리는 같다.

```ts
// ✅ 적응형 폴링: 변화가 없으면 간격을 늘리고(exponential backoff), 상한을 둔다
async function waitForJob(id: string, signal?: AbortSignal) {
  let interval = 50;
  const maxInterval = 2000;
  while (!signal?.aborted) {
    const { status } = await db.one('SELECT status FROM jobs WHERE id = $1', [id]);
    if (status === 'done') return;
    await new Promise((r) => setTimeout(r, interval));
    interval = Math.min(interval * 2, maxInterval);
  }
}
```

조건이 이벤트로 표현 가능하다면 폴링 자체를 없앤다. PostgreSQL의 `LISTEN/NOTIFY`를 쓰면 변화가 생긴 순간에만 콜백이 실행되므로, 폴링 간격에 따른 지연과 불필요한 조회가 모두 사라진다.

```ts
// ✅ 이벤트 기반: 변화가 생긴 순간에만 알림을 받는다 (폴링 없음)
const conn = await db.connect({ direct: true }); // LISTEN 전용 연결
conn.client.on('notification', (msg) => {
  if (msg.channel === 'job_done' && msg.payload === id) {
    // 완료 처리
  }
});
await conn.none('LISTEN job_done');
```

반대로 "데드라인 경과"처럼 시간이 흘러서 성립하는 조건은 이벤트로 표현할 수 없다. 이 경우 Node.js에서는 폴링 루프 대신 `setTimeout`으로 커널 타이머에 위임하는 것이 C의 `timerfd`에 해당한다.

```ts
// ✅ 시간 기반 조건: 폴링 대신 타이머에 위임 (deadline까지 CPU를 쓰지 않는다)
function onDeadline(deadlineMs: number, cb: () => void) {
  const timer = setTimeout(cb, Math.max(0, deadlineMs - Date.now()));
  return () => clearTimeout(timer); // 조건이 먼저 바뀌면 취소
}
```