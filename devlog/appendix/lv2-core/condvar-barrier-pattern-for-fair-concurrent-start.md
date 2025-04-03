# 조건변수 기반 배리어 패턴으로 공정한 동시 시작 만들기

**Lv2-core / 동시성**

**프로젝트에서** `src/routine.c:10-37`의 `wait_for_start()`와 `src/run.c:39-76`의 `release_start()`가 한 쌍으로 배리어(barrier)를 이룬다. 모든 철학자 스레드가 `pthread_create`된 뒤, 전원이 "나 준비됐다"(`ready_count`)를 알릴 때까지 부모가 기다렸다가, 그제서야 하나의 시각(`start_ms`)을 모두에게 동시에 나눠주고 broadcast로 풀어준다.

```c
// src/run.c:62-69 (release_start 발췌)
start_ms = philo_now_ms();
table->start_ms = start_ms;
i = 0;
while (i < table->config.number)
{
    table->philos[i].last_meal_ms = start_ms;
    i++;
}
```

**일반적으로** 배리어는 "N개의 실행 주체가 모두 특정 지점에 도달할 때까지 아무도 그 지점을 지나가지 못하게 막는" 동기화 도구다. POSIX에도 `pthread_barrier_t`라는 표준 구현이 있지만(모든 플랫폼에 있는 건 아니다. 예를 들어 macOS는 기본 제공하지 않는다), 이 프로젝트는 조건변수와 카운터(`ready_count`)만으로 같은 효과를 직접 구현했다. 핵심 아이디어는 두 단계다: (1) 각 참여자가 도착하면 카운터를 올리고 신호를 보낸다, (2) 카운터가 목표에 도달했는지 감시하는 쪽(또는 마지막 도착자)이 전원을 동시에 풀어준다.

**모범사례**:  
`pthread_barrier_t`를 썼다면 이 코드는 훨씬 짧아졌을 것이다. 직접 구현을 택한 이유는 두 가지다. 

1. 과제 관례상 이식성 낮은 non-POSIX-표준 확장(`pthread_barrier_t`는 POSIX 표준에는 있지만 실제로 모든 타겟 플랫폼에 구현되어 있지 않다)에 의존하지 않는 편이 안전했다. 

2. 표준 배리어는 "N명이 도착하면 전원 통과"만 시켜줄 뿐, 이 프로젝트처럼 "그 시점의 시각을 모두에게 동일하게 나눠준다"는 추가 작업(부수 효과)을 함께 원자적으로 해주지 않는다. 결국 표준 배리어를 썼어도 그 주변에 이 로직을 직접 덧붙여야 했을 것이다.

이 배리어 구현에서 가장 쉽게 빠지는 함정은 "정상 경로만" 구현하는 것이다. 만약 스레드 생성 중간에 `pthread_create`가 실패하면 어떻게 되는가? 이미 만들어진 스레드들은 `wait_for_start()`에서 여전히 `cond_wait`로 잠들어 있다. 목표 인원수(`table->config.number`)에 도달하지 못했으므로 정상적으로는 절대 풀리지 않는다. `release_start(table, 1)`이 `should_end` 플래그로 이 교착을 깬다. "목표 인원에 도달하길 기다리지 말고, 강제로 지금 풀어주되 `ended`를 세워서 실제 작업은 시작하지 않게 하라"는 두 번째 출구다. 배리어를 설계할 때는 "전원이 정상적으로 도착하는 경우"뿐 아니라 "일부만 도착했는데 더 이상 기다릴 수 없는 경우"의 출구를 항상 같이 설계해야 한다. 그렇지 않으면 그 배리어는 실패 상황에서 그대로 교착의 원인이 된다.

## 웹에서는 어디에 나타나는가

Java의 `CyclicBarrier`나 `CountDownLatch`, Go의 `sync.WaitGroup`은 이 프로젝트가 직접 만든 것과 같은 문제(N개가 모일 때까지 기다렸다가 동시에 진행)를 표준 라이브러리 수준에서 제공한다. 이런 도구를 쓸 수 있는 환경이라면 조건변수를 직접 다루는 이 코드 전체가 필요 없어진다. 다만 그 경우에도 "카운터가 목표에 못 미친 채 참여자 중 일부가 실패해버리면 어떻게 할 것인가"라는 질문 자체는 여전히 개발자가 답해야 한다. `CyclicBarrier`는 이 상황에서 `BrokenBarrierException`을 던져 호출자가 직접 처리하게 하는데, 이는 이 프로젝트의 `should_end`/`run_error` 플래그가 하는 역할과 본질적으로 같다.

Node.js에는 이런 배리어가 기본 제공되지 않지만, Promise로 같은 구조를 만들 수 있다. 각 참여자는 도착을 알리고 `await`로 대기하며, 마지막 도착자가 대기 중인 Promise를 한 번에 resolve한다. 실패 경로는 `reject`가 맡는다.

```ts
// 배리어: n명이 모이면 전원을 동시에 풀어주고, 그 시점의 startMs를 모두에게 동일하게 전달한다
function createBarrier(n: number) {
  let arrived = 0;
  let release!: (startMs: number) => void;
  let abort!: (err: Error) => void;
  const gate = new Promise<number>((res, rej) => {
    release = res;
    abort = rej;
  });

  return {
    // 참여자: 도착을 알리고 풀릴 때까지 대기 (C의 ready_count++ 후 cond_wait)
    async wait(): Promise<number> {
      arrived++;
      if (arrived === n) release(Date.now()); // 마지막 도착자가 전원을 풀어줌 (broadcast)
      return gate;
    },
    // 실패 경로: 목표 인원이 못 모여도 대기자들을 깨운다 (C의 release_start(table, 1))
    fail(err: Error) {
      abort(err);
    },
  };
}

const barrier = createBarrier(3);

async function philosopher(id: number) {
  const startMs = await barrier.wait(); // 전원이 같은 startMs를 받는다
  // ... 시작 시각 기준으로 작업 수행
}
```

`fail()`을 두지 않으면 일부 참여자가 도착하지 못했을 때 나머지 `await`는 영원히 풀리지 않는다. 표준 배리어를 쓰든 직접 만들든, 실패 시의 출구를 함께 설계해야 한다는 점은 동일하다.