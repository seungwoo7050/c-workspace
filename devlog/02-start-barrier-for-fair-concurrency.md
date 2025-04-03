# Dev Log 02. A condition-variable barrier for a fair simultaneous start

## 1. `fix(thread): 시작 장벽으로 기준 시각 통일` / `test(thread): 지연된 작업자의 공통 시작 시각 검증` / `test(thread): 시작 대기 실패 전파 검증`

첫 커밋이 조건변수 기반 시작 장벽(barrier)을 도입하고, 이어지는 두 테스트가 정확히 그 장벽의 정상 동작과 실패 전파 경로를 검증한다.

### 1. 문제: `pthread_create`가 스레드마다 걸리는 시간이 다르다

스레드를 만들 때마다 그 자리에서 `last_meal_ms`를 찍었다. 이러면 나중에 만들어진 철학자일수록 먼저 만들어진 철학자보다 실제로 뛰기 시작하는 시각이 늦어, 애초에 불공평한 조건에서 사망 카운트다운이 시작된다.

```c
int	philo_run(t_table *table)
{
    ...
	  while (i < table->config.number)
	  {
        pthread_mutex_lock(&table->state_mutex);
        table->philos[i].last_meal_ms = table->start_ms;
        pthread_mutex_unlock(&table->state_mutex);
		    ...
}
```

### 2. 2단계 구조: 전원 생성 → 전원 동시 해제

```c
// src/routine.c
static int	wait_for_start(t_philo *philo)
{
    t_table *table;
    int ended;

    table = philo->table;
    pthread_mutex_lock(&table->state_mutex);
    table->ready_count++;
    // pthread_cond_broadcast: 이 조건변수를 기다리며 잠들어 있는 모든 스레드를 한 번에 깨운다
    // (신호를 하나만 깨우는 pthread_cond_signal과 달리, 대기 중인 전원에게 알린다)
    pthread_cond_broadcast(&table->start_cond);
    while (!table->start_released)
    {
        // pthread_cond_wait(cond, mutex): 이 mutex를 잠깐 풀고 잠들었다가, 누군가 broadcast/signal로 깨우면 mutex를 다시 잠근 뒤 돌아온다
        // "락을 쥔 채로 그냥 기다리면" 다른 스레드가 그 락을 못 잡아 아무도 상태를 못 바꾸므로, 대기 중엔 락을 풀어주는 이 함수가 필요하다
        if (pthread_cond_wait(&table->start_cond, &table->state_mutex) != 0)
        {
            table->run_error = 1;
            table->ended = 1;
            table->start_released = 1;
            pthread_cond_broadcast(&table->start_cond);
        }
    }
    ended = table->ended;
    pthread_mutex_unlock(&table->state_mutex);
    return (ended);
}
```

```c
// src/run.c
static int	release_start(t_table *table, int should_end)
{
	...
	pthread_mutex_lock(&table->state_mutex);
	while (!should_end && table->ready_count < table->config.number)
	{
		if (pthread_cond_wait(&table->start_cond,
				&table->state_mutex) != 0)
		{
			table->run_error = 1;
			should_end = 1;
			status = PHILO_ERR;
		}
	}
	...
	start_ms = philo_now_ms();
	table->start_ms = start_ms;
	i = 0;
	while (i < table->config.number)
	{
		table->philos[i].last_meal_ms = start_ms;
		i++;
	}
	if (should_end)
		table->ended = 1;
	table->start_released = 1;
	pthread_cond_broadcast(&table->start_cond);
	pthread_mutex_unlock(&table->state_mutex);
	return (status);
}
```

`pthread_create`를 스레드 수만큼 모두 반복해서 부른 뒤에야 `release_start()`로 전원을 동시에 풀어준다. 각 철학자 스레드는 `wait_for_start()`에서 "나 준비됐다"를 `ready_count++`와 broadcast로 알린 뒤, `start_released`가 세워질 때까지 `cond_wait`로 잠들어 있는다. 부모(실행 스레드)는 모든 철학자의 `ready_count`가 인원수에 도달한 걸 확인한 뒤에야 `start_ms`를 "지금"으로 딱 한 번 찍고, 그 하나의 시각을 모든 철학자의 `last_meal_ms`에 일괄로 써넣은 다음 `start_released = 1`과 broadcast로 전원을 한 번에 풀어준다. 각 철학자가 스스로 깨어난 시점에 각자 `last_meal_ms`를 기록하게 두면, 스레드마다 깨어나는 타이밍이 OS 스케줄러에 따라 미묘하게 달라 사망 판정 기준선이 철학자마다 달라지는데, 부모가 "모두를 대표하는 하나의 시각"으로 한 번에 기록하면 이 문제가 사라진다.

`pthread_cond_wait`가 실패하는(드물지만 이론상 가능한) 경우까지 두 곳 모두에서 처리한다. 실패를 무시하고 계속 대기하면 `start_released`가 영원히 안 켜져서 그 스레드가 논리적으로 멈춰버릴 수 있어서, `run_error`를 세우고 강제로 `ended`/`start_released`를 켜서 자신과 다른 대기 중인 스레드를 모두 풀어준다.

### 3. `pthread_create` 실행 순서를 조작해 지연된 스레드 시나리오를 만든다

```c
// tests/start_barrier.c
// 이 함수의 이름과 매개변수 타입이 진짜 pthread_create와 완전히 똑같다는 점이 핵심이다
// 빌드 시 "이 이름으로 호출된 pthread_create는 전부 이 함수로 바꿔치기"하는 방식(심볼 오버라이드)으로 
// 실제 라이브러리 대신 이 가짜 함수가 호출되게 만들어, 스레드 생성 타이밍을 테스트가 마음대로 조작할 수 있다
int	test_pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*routine)(void *), void *arg)
{
    int	index;

    index = g_created;
    g_starts[index].routine = routine;
    g_starts[index].arg = arg;
    g_starts[index].delay_us = (index == 4) * 150000;
    ...
}
```

`-Dpthread_create=test_pthread_create`로 실제 `pthread_create`를 가로채, 5번째로 생성되는 스레드(`index == 4`)만 실제 루틴을 시작하기 전에 150ms를 더 자게 만들었다. 그리고 `test_pthread_create` 자신도 모든 스레드가 만들어질 때까지는 문(gate)을 잠가둬서, "5개 전부 만들어진 뒤에야 동시에 풀린다"는 barrier 자체의 동작을 시뮬레이션 안에 재현했다. 이 인위적인 150ms 지연이 있어도 `philo_run`이 `PHILO_OK`로 끝나고 전원이 `full_count`/`ready_count`를 채우는지를 확인하면, barrier가 실제로 "가장 늦게 준비된 스레드까지 기다렸다가" 공정하게 출발시킨다는 걸 검증하는 셈이다.

### 4. `pthread_cond_wait` 자체의 실패를 주입해 전파 경로를 확인한다

```c
// tests/worker_wait_failure.c
int	test_pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
    ...
    // EINVAL: "인자가 잘못됐다"는 뜻의 표준 에러 코드(errno 값 중 하나)
    // 여기서는 진짜 에러가 난 게 아니라, 테스트가 실패 상황을 흉내내려고 일부러 이 값을 반환한다
    if (should_fail)
        return (EINVAL);
    return (pthread_cond_wait(cond, mutex));
}
```

`wait_for_start()` 안의 첫 `pthread_cond_wait` 호출 한 번만 실패시켜서, `run_error`가 세워지고 `philo_run()`이 `PHILO_ERR`을 반환하는지 확인한다.

## 정리

동시에 시작해야 하는 작업은 전원이 준비됐다는 사실을 한곳에서 확인한 뒤 하나의 기준값을 나눠주고 한 번에 풀어줘야 공정하다. 배리어는 정상 경로만으로는 완성되지 않는다. 일부가 도착하지 못했을 때 대기자를 풀어주는 출구가 없으면 실패 상황에서 그대로 교착이 된다.

웹에서는 여러 비동기 작업을 모아 한 시점에 출발시키는 조율이 같은 구조다. Promise 기반 배리어와 실패 출구는 `appendix/lv2-core/condvar-barrier-pattern-for-fair-concurrent-start.md`에, 조건변수 자체의 사용법은 `appendix/lv1/pthread-condition-variable-basics.md`에 있다.
