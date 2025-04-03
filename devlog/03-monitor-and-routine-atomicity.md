# Dev Log 03. Making the terminal decision atomic, and excluding an interrupted meal

## 19. `fix(monitor): 종료 상태와 사망 로그를 원자적으로 확정` / `test(monitor): 완료 상태와 오래된 사망 판정 검증`

### 1. `philo_log_death()`를 `philo_try_log_death()`로 바꾼다

```c
// 이전 state.c
void	philo_log_death(t_philo *philo)
{
    ...
    pthread_mutex_lock(&table->state_mutex);
    should_print = !table->ended;
    table->ended = 1;
    pthread_mutex_unlock(&table->state_mutex);
    if (should_print)
    {
        pthread_mutex_lock(&table->print_mutex);
        timestamp = philo_now_ms() - table->start_ms;
        printf("%lld %d died\n", (long long)timestamp, philo->id);
        pthread_mutex_unlock(&table->print_mutex);
    }
}

// 이후
int	philo_try_log_death(t_philo *philo)
{
    ...
    // 락 두 개를 같이 쓸 때는 항상 같은 순서로 잠가야 한다(여기선 print_mutex → state_mutex)
    // 어떤 스레드는 A→B 순서로, 다른 스레드는 B→A 순서로 잠그면 서로가 서로의 락을 기다리며
    // 영원히 멈추는 교착상태(deadlock)가 생길 수 있어서, 순서를 코드 전체에서 통일해둔다
    pthread_mutex_lock(&table->print_mutex);
    pthread_mutex_lock(&table->state_mutex);
    now = philo_now_ms();
    if (!table->ended && now - philo->last_meal_ms >= table->config.time_to_die)
    {
        table->ended = 1;
        timestamp = now - table->start_ms;
        should_print = 1;
    }
    pthread_mutex_unlock(&table->state_mutex);
    if (should_print)
        // %lld: long long 타입 값을 출력할 때 쓰는 형식 지정자(%d는 int용, %ld는 long용)
        // (long long)으로 먼저 캐스팅해 타입과 형식 지정자를 맞춰준다
        printf("%lld %d died\n", (long long)timestamp, philo->id);
    pthread_mutex_unlock(&table->print_mutex);
    return (should_print);
}
```

이전 버전은 `philo_monitor()`가 `find_dead_philo()`로 "죽은 것처럼 보이는" 철학자를 찾아낸 시점과 실제로 사망을 확정하는 시점 사이에 이미 락을 놓았다 다시 잡는 구조였는데, `philo_log_death()` 자신은 그 사이에 상황이 바뀌었는지(예: 그 철학자가 마침 포크를 얻어 `last_meal_ms`를 갱신했는지)를 재확인하지 않고 무조건 `table->ended = 1`을 세워버렸다. 고친 뒤에는 `now - philo->last_meal_ms >= time_to_die` 조건을 락을 쥔 채 다시 한번 검사한다 → `find_dead_philo`가 찾은 대상은 "재확인 대상 후보"일 뿐 그 자체로 사망 확정이 아니게 됐다.

`print_mutex`를 `state_mutex`보다 항상 먼저 잠그는 순서도 이 함수와 `philo_log()` 양쪽에서 일관되게 지킨다. 락 순서가 파일마다 뒤바뀌면 서로 다른 스레드가 반대 순서로 두 락을 요구하는 전형적인 데드락 상황이 생기기 때문이다.

```c
// src/monitor.c
void	philo_monitor(t_table *table)
{
    ...
    while (1)
    {
        now = philo_now_ms();
        pthread_mutex_lock(&table->state_mutex);
        if (table->ended)
        {
            pthread_mutex_unlock(&table->state_mutex);
            return ;
        }
        if (all_meals_done(table))
        {
            table->ended = 1;
            pthread_mutex_unlock(&table->state_mutex);
            return ;
        }
        dead = find_dead_philo(table, now);
        pthread_mutex_unlock(&table->state_mutex);
        if (dead != NULL && philo_try_log_death(dead))
            return ;
        usleep(500);
    }
}
```

`philo_monitor()` 쪽도 루프 조건을 `while (!philo_has_ended(table))`에서 `while (1)` + 루프 맨 앞의 명시적 `ended` 확인으로 바꿨다. 그리고 "모든 철학자가 식사를 완료했다"는 판정과 `table->ended = 1`을 세우는 것을 같은 락 구간 안에서 원자적으로 처리한다. 이전처럼 락을 풀고 나서 `philo_finish()`를 별도로 부르면, 그 사이의 짧은 틈에 다른 스레드가 끼어들 여지가 생긴다.

### 2. `pthread_mutex_unlock`을 가로채 "락이 풀린 바로 그 순간"에 상태를 조작한다

```c
// tests/terminal_state.c
int	test_mutex_unlock(pthread_mutex_t *mutex)
{
    int	status;

    status = pthread_mutex_unlock(mutex);
    if (!g_injected && mutex == &g_table->state_mutex)
    {
        g_injected = 1;
        g_ended_at_unlock = g_table->ended;
        if (g_mode == MODE_STALE_DEATH)
        {
            pthread_mutex_lock(&g_table->state_mutex);
            g_table->philos[0].last_meal_ms = philo_now_ms();
            g_table->full_count = 1;
            pthread_mutex_unlock(&g_table->state_mutex);
        }
    }
    return (0);
}
```

이 테스트는 `pthread_mutex_unlock` 자체를 가로채서, `state_mutex`가 처음 풀리는 바로 그 순간에 끼어들어 상태를 바꾼다. `MODE_COMPLETION`에서는 "락이 풀린 시점에 이미 `ended`가 세워져 있었는가"를 확인해 완료 판정이 락 구간 안에서 커밋됐는지를 검증하고, `MODE_STALE_DEATH`에서는 `find_dead_philo`가 죽음 후보를 찾아낸 직후, `philo_try_log_death`가 재확인하기 전에 그 철학자의 `last_meal_ms`를 갱신하고 `full_count`를 채워버려서 "방금 막 살아난" 상황을 인위적으로 만든다.

`ended`가 락을 쥔 채로 세워졌는지 확인했고, 오래된 사망 판정 케이스에서는 `philo_try_log_death`의 재확인 로직이 실제로 작동해 `died` 로그가 찍히지 않고 `full_count`가 세워진 완료 경로로 대신 확정되는지 확인했다(`grep -q 'died' ... && fail 'stale death was printed'`).

## 20. `fix(routine): 중단된 식사를 완료 횟수에서 제외` / `test(routine): 중단된 식사의 카운터 불변식 검증`

### 1. `philo_sleep_ms`와 `record_meal_done`이 실패를 반환하게 한다

```c
// src/routine.c
static int	eat_once(t_philo *philo)
{
    lock_forks(philo);
    if (philo_has_ended(philo->table))
    {
        unlock_forks(philo);
        return (PHILO_ERR);
    }
    record_meal_start(philo);
    philo_log(philo, "is eating");
    if (philo_sleep_ms(philo->table, philo->table->config.time_to_eat) != PHILO_OK || record_meal_done(philo) != PHILO_OK)
    {
        unlock_forks(philo);
        return (PHILO_ERR);
    }
    unlock_forks(philo);
    return (PHILO_OK);
}
```

`philo_sleep_ms()`가 `void`에서 `int`로 바뀌어, 자는 도중에 시뮬레이션이 끝나면(`ended`가 세워지면) `PHILO_ERR`을 반환하게 됐다. `record_meal_done()`도 마찬가지로 `table->ended`가 이미 세워져 있으면 `philo->meals`를 증가시키지 않고 `PHILO_ERR`을 반환한다. 이 둘 중 하나라도 실패하면 `eat_once()`는 식사를 "완료"로 카운트하지 않고 그대로 중단한다.

이전 버전은 자는 도중에 시뮬레이션이 끝나도 `philo_sleep_ms`가 그냥 `break`로 조용히 빠져나온 뒤 `record_meal_done()`을 그대로 호출했다. 즉 실제로는 `time_to_eat`을 다 채우지 못하고 중간에 끊긴 식사인데도 `meals` 카운터가 올라가고, 운이 나쁘면 그게 `must_eat` 목표 달성으로 잘못 집계될 수 있었다.

### 2. `philo_sleep_ms`를 가짜로 바꿔치기해 "자는 도중 중단"을 강제로 재현한다

```c
// tests/interrupted_meal.c
int	test_philo_sleep_ms(t_table *table, int64_t duration_ms)
{
    pthread_mutex_lock(&table->state_mutex);
    table->ended = 1;
    pthread_mutex_unlock(&table->state_mutex);
    interrupted = 1;
    return (PHILO_ERR);
}
```

`-Dphilo_sleep_ms=test_philo_sleep_ms`로 실제 대기 로직 전체를 건너뛰고, 그 자리에서 곧바로 `table->ended = 1`을 세우면서 `PHILO_ERR`을 반환하게 만들었다. "먹는 도중 시뮬레이션이 끝났다"는 상황을 실제로 몇백 ms를 기다리지 않고도 즉시, 결정적으로 재현하는 방법이다.

`philo_routine()`을 이 상태로 한 번 호출한 뒤 `table.philos[0].meals`와 `table.full_count`가 둘 다 0으로 남아있는지 확인했다. 중단된 식사가 완료 횟수에 전혀 반영되지 않았음을 뜻한다.