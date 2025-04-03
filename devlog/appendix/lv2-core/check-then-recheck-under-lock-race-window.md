# 락 밖에서 후보를 찾고 락 안에서 재확인하는 TOCTOU 방지 패턴

**Lv2-core / 동시성**

**프로젝트에서** `src/monitor.c:11-59`의 `philo_monitor()`가 `state_mutex`를 푼 상태에서 `find_dead_philo()`로 사망 후보를 찾은 뒤, `src/state.c:49-74`의 `philo_try_log_death()`가 `state_mutex`를 다시 잠근 채로 그 후보의 사망 조건을 한 번 더 검사하고 나서야 `table->ended`를 확정한다.

```c
// src/monitor.c:50-58
dead = find_dead_philo(table, now);
pthread_mutex_unlock(&table->state_mutex);
if (dead != NULL && philo_try_log_death(dead))
	  return ;
```

```c
// src/state.c:59-68 (philo_try_log_death 발췌)
pthread_mutex_lock(&table->print_mutex);
pthread_mutex_lock(&table->state_mutex);
now = philo_now_ms();
if (!table->ended && now - philo->last_meal_ms >= table->config.time_to_die)
{
	  table->ended = 1;
	  ...
}
```

**일반적으로** TOCTOU(Time-Of-Check to Time-Of-Use)는 "확인한 시점"과 "그 확인 결과를 실제로 사용하는 시점" 사이에 상태가 바뀔 수 있는데도 확인 결과를 그대로 믿어버리는 클래스의 버그다. 락으로 보호되는 공유 상태를 다룰 때 흔한 실수는 "락을 잡고 조건을 확인한 뒤, 락을 풀고 나서 그 결과에 따라 무언가를 확정한다"는 것이다. 락을 푸는 순간부터 다른 스레드가 끼어들 수 있으므로, 확인한 조건이 여전히 참이라는 보장이 사라진다. 안전하게 하려면 "확정" 자체를 락 재확인과 같은 임계구역 안에서 원자적으로 처리해야 한다.

**모범사례**:  
굳이 락을 풀었다 다시 잡는 2단계 구조를 쓴 이유는 성능이다. `find_dead_philo()`는 모든 철학자를 순회하며 여러 비교 연산을 하는데, 이걸 락을 쥔 채로 하면 그동안 다른 철학자 스레드들이 `last_meal_ms`를 갱신하지 못해 락 경합이 커진다. 그래서 "넓은 탐색은 락 밖에서 느슨하게, 좁은 확정은 락 안에서 한 번만 원자적으로"라는 절충을 택했다. `find_dead_philo`가 찾은 결과는 확정이 아니라 재확인이 필요한 "후보"일 뿐이라는 걸 타입이 아니라 함수 이름(`try_log_death`의 `try`)과 반환값으로 표현한다. 더 엄격한 시스템이라면 이런 이름 규약 대신 "확정되지 않은 후보"를 나타내는 별도 타입으로 감싸 컴파일 타임에 실수를 막기도 한다.

재확인이 없으면 다음 순서로 거짓 사망 판정이 나온다.

1. `find_dead_philo`가 철학자 3을 "죽은 것 같다"고 찾아낸다(`now - last_meal_ms >= time_to_die`가 참이었을 때).
2. `state_mutex`가 풀린다.
3. 철학자 3의 스레드가 마침 그 순간 포크를 얻어 `record_meal_start()`로 `last_meal_ms`를 방금 갱신한다(철학자 3은 실제로는 살아있다).
4. 재확인 없이 `philo_try_log_death`가 그냥 `table->ended = 1`을 세우고 "3 died"를 찍었다면, 이미 다시 식사를 시작한 철학자를 죽었다고 오보하는 것이다.

`philo_try_log_death` 내부의 `now - philo->last_meal_ms >= table->config.time_to_die`가 락을 쥔 채 다시 계산되기 때문에, 3번에서 갱신된 `last_meal_ms`가 이 재확인에 그대로 반영되어 거짓 사망 판정을 막는다. `tests/terminal_state.c`의 `MODE_STALE_DEATH` 케이스가 정확히 이 시나리오를 인위적으로 재현해서 검증한다.

## 웹에서는 어디에 나타나는가

같은 TOCTOU 원리가 DB 트랜잭션에서도 그대로 나타난다. `SELECT`로 재고를 확인(check)한 뒤 별도의 `UPDATE`로 차감(act)하면, 그 사이에 다른 트랜잭션이 끼어들어 재고를 먼저 차감해버릴 수 있다.

Node.js는 싱글 스레드 이벤트 루프라 C처럼 스레드가 동시에 메모리를 건드리지는 않는다. 하지만 `await`로 DB I/O를 기다리는 동안 다른 요청의 코드가 실행되므로, `await` 사이가 곧 "락을 푼 구간"과 같은 역할을 한다. 여러 서버 인스턴스가 같은 DB를 쓰는 경우에는 더더욱 그렇다.

```ts
// ❌ check와 act가 분리됨: 두 요청이 동시에 stock=1을 읽으면 둘 다 통과한다
const { stock } = await db.one('SELECT stock FROM items WHERE id = $1', [id]);
// ← 이 await 사이에 다른 요청이 끼어들어 재고를 먼저 차감할 수 있다
if (stock >= 1) {
  await db.none('UPDATE items SET stock = stock - 1 WHERE id = $1', [id]);
}
```

흔한 해법은 확인과 사용을 하나의 원자적 SQL 문으로 합치는 것이다. `WHERE` 조건이 C 코드의 "락 안에서의 재확인" 역할을 하고, 영향받은 행 수로 성공 여부를 판단한다.

```ts
// ✅ 확인과 확정을 한 문장으로: 영향받은 행이 0이면 재고 부족
const result = await db.result(
  'UPDATE items SET stock = stock - 1 WHERE id = $1 AND stock >= 1',
  [id]
);
if (result.rowCount === 0) throw new Error('out of stock');
```

또 다른 해법은 버전 컬럼을 비교하는 낙관적 락(optimistic lock)이다. 읽은 시점의 버전이 갱신 시점에도 그대로일 때만 쓰기를 허용하고, 그 사이 다른 트랜잭션이 바꿨다면 갱신이 0행에 그치므로 다시 읽고 재시도한다.

```ts
// ✅ 낙관적 락: 읽은 시점의 version이 그대로일 때만 갱신
const result = await db.result(
  'UPDATE items SET stock = $1, version = version + 1 WHERE id = $2 AND version = $3',
  [newStock, id, readVersion]
);
if (result.rowCount === 0) {
  // 누군가 먼저 바꿨음 → 다시 읽고 재시도
}
```

이 프로젝트의 재확인 패턴과 원리는 같다. "확인"과 "확정"을 서로 다른 두 단계로 분리하되, 확정하는 순간에는 반드시 그 조건을 원자적으로 다시 검사한다.