# 식사하는 철학자 문제의 일관된 락 순서와 교착 회피

**Lv2-core / 동시성**

**프로젝트에서** `src/routine.c:44-60`의 `lock_forks()`가 철학자 id의 홀짝에 따라 포크를 집는 순서를 반대로 뒤집는다.

```c
// src/routine.c:44-60
static void	lock_forks(t_philo *philo)
{
    if (philo->id % 2 == 0)
    {
        pthread_mutex_lock(philo->right_fork);
        philo_log(philo, "has taken a fork");
        pthread_mutex_lock(philo->left_fork);
        philo_log(philo, "has taken a fork");
    }
    else
    {
        pthread_mutex_lock(philo->left_fork);
        philo_log(philo, "has taken a fork");
        pthread_mutex_lock(philo->right_fork);
        philo_log(philo, "has taken a fork");
    }
}
```

같은 원칙이 `src/state.c:33`(`philo_log`)과 `src/state.c:59-60`(`philo_try_log_death`)에서도 반복된다. 두 함수 모두 `print_mutex`를 잠근 뒤에야 `state_mutex`를 잠그는 순서를 지킨다. 서로 다른 두 개의 뮤텍스를 여러 함수에서 다루면서도, 어느 함수에서든 항상 같은 순서로 잠근다는 규칙 하나만 지켰다.

**일반적으로** 교착(deadlock)이 성립하려면 네 가지 조건(상호 배제, 점유 대기, 비선점, 순환 대기)이 모두 필요한데, 그중 순환 대기(circular wait)만 깨도 나머지 조건과 무관하게 교착이 원천적으로 불가능해진다. 순환 대기를 깨는 가장 흔한 방법이 "모든 락에 전역적으로 일관된 순서를 매기고, 어떤 스레드도 그 순서를 거슬러 잠그지 않게 한다"는 것이다. N개의 자원이 원형으로 배치된 전형적인 식사하는 철학자 문제에서는, 모두가 "낮은 인덱스 포크 → 높은 인덱스 포크" 순서로 통일하거나(이 프로젝트처럼 인접한 두 철학자의 순서를 어긋나게 하는 것도 같은 효과), 자원 개수만큼 있는 락 대신 하나의 상위 락으로 포크 획득 전체를 감싸는 방법도 있다.

**모범사례**:  
홀짝 반전은 정확히 이 "원형 5개(또는 N개) 철학자" 구조에서만 순환을 끊는다는 걸 보장한다. 만약 포크가 원형이 아니라 임의의 그래프로 공유되는 시스템(예: 여러 종류의 락을 상황에 따라 다른 순서로 획득해야 하는 범용 트랜잭션 매니저)이라면, id의 홀짝 같은 국소적인 트릭 대신 모든 락에 전역 순번을 매기고 그 순번 순서로만 획득하도록 강제하는 더 일반적인 메커니즘(락 계층, 타임아웃 후 전체 롤백 등)이 필요하다. 이 프로젝트는 자원의 배치(N명이 원탁에 앉은 고정된 구조)를 미리 알고 있기 때문에 이렇게 단순한 트릭으로 충분했다.

이 코드는 왼쪽 포크를 집고 나서 오른쪽 포크를 기다리는 동안, 오른쪽 포크가 안 잡히면 왼쪽 포크를 도로 놓고 기다리는 식의 "선점(preemption)" 전략을 쓰지 않는다. 두 뮤텍스를 순서대로 블로킹 방식(`pthread_mutex_lock`)으로 잠그기만 하고, 실패 시 되돌리는 로직이 없다. 상호 배제와 점유 대기 조건 자체는 그대로 유지한 채, 오직 순환 대기 조건만 없앤 것이다. 만약 이 프로젝트가 `pthread_mutex_trylock`으로 "안 되면 즉시 포기하고 재시도"하는 방식을 썼다면 순환 대기가 없어도 라이브락(livelock, 계속 재시도만 하며 아무도 진전하지 못하는 상태)의 위험이 생길 수 있다. 락 순서 고정 쪽이 더 단순하고 예측 가능한 선택이다.

## 웹에서는 어디에 나타나는가

DB 트랜잭션이나 분산 락 시스템에서는 락 순서를 전역적으로 통제하기 어려운 경우(요청마다 필요한 자원 집합이 동적으로 달라짐)가 많다. 이런 환경에서는 순서 고정 대신 "일정 시간 안에 필요한 락을 다 못 얻으면 지금까지 얻은 걸 전부 풀고 처음부터 재시도한다"(wait-die, wound-wait 같은 스킴)는 방식을 쓴다. 이 프로젝트가 그 방식을 쓰지 않은 이유는 단순하다. 포크의 구조(원형, N개)가 컴파일 타임부터 고정돼 있어서 락 순서를 정적으로 미리 정할 수 있고, 그러면 런타임에 타임아웃과 롤백이라는 훨씬 무거운 메커니즘이 아예 필요 없어지기 때문이다.

Node.js 백엔드에서 이 문제는 주로 DB 행 락에서 나타난다. 이체처럼 두 행을 잠그는 요청이 서로 반대 방향으로 들어오면 포크를 반대 순서로 집는 철학자와 똑같은 교착이 생긴다.

```ts
// ❌ 요청마다 락 순서가 달라짐: transfer(A→B)와 transfer(B→A)가 동시에 실행되면 교착
async function transfer(from: number, to: number, amount: number) {
  await db.tx(async (t) => {
    await t.none('SELECT 1 FROM accounts WHERE id = $1 FOR UPDATE', [from]);
    await t.none('SELECT 1 FROM accounts WHERE id = $1 FOR UPDATE', [to]);
    await t.none('UPDATE accounts SET balance = balance - $1 WHERE id = $2', [amount, from]);
    await t.none('UPDATE accounts SET balance = balance + $1 WHERE id = $2', [amount, to]);
  });
}
```

락 순서를 코드에서 통제할 수 있다면 이 프로젝트와 같은 해법이 그대로 통한다. 잠글 행을 id 오름차순으로 정렬해서 획득하면 순환 대기가 사라진다.

```ts
// ✅ 전역 순서 고정: 항상 id가 낮은 행부터 잠근다 (C의 "낮은 인덱스 포크 → 높은 인덱스 포크")
await t.none(
  'SELECT 1 FROM accounts WHERE id = ANY($1) ORDER BY id FOR UPDATE',
  [[from, to]]
);
```

순서를 통제할 수 없다면 타임아웃과 전체 롤백 후 재시도가 표준 해법이다. DB가 교착을 감지해 한쪽 트랜잭션을 강제로 롤백하거나(`40P01`), 락 대기 시간이 초과되면(`55P03`) 에러를 던지고, 애플리케이션은 트랜잭션 전체를 처음부터 다시 실행한다.

```ts
// ✅ 타임아웃 + 전체 롤백 + 재시도: 얻은 락을 모두 풀고 처음부터 다시 시도한다
async function withRetry<T>(fn: () => Promise<T>, maxAttempts = 5): Promise<T> {
  for (let attempt = 1; ; attempt++) {
    try {
      return await fn();
    } catch (err: any) {
      const retryable = err.code === '40P01' || err.code === '55P03';
      if (!retryable || attempt >= maxAttempts) throw err;
      // 재시도 시각을 무작위로 흩뜨려서 같은 타이밍에 다시 충돌하는 라이브락을 피한다
      await new Promise((r) => setTimeout(r, Math.random() * 50 * attempt));
    }
  }
}

await withRetry(() =>
  db.tx(async (t) => {
    await t.none("SET LOCAL lock_timeout = '500ms'"); // 500ms 안에 못 얻으면 55P03
    // ... 필요한 행을 순서 상관없이 FOR UPDATE로 잠그고 작업
  })
);
```

재시도 간격에 무작위성(jitter)을 넣는 이유는 이 문서의 `trylock` 논의와 같다. 모두가 같은 간격으로 재시도하면 순환 대기가 없어도 계속 충돌만 반복하는 라이브락이 생길 수 있다.