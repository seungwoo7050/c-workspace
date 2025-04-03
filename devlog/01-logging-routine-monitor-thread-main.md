# Dev Log 01. Logging, the eating routine, the monitor, threads and main

## 1. `feat(log): 상태 로그의 동시 출력 보호`

여러 철학자 스레드가 동시에 `printf`를 부르면 한 줄이 다른 줄 중간에 끼어드는(interleaving) 출력이 나올 수 있어서, 로그 출력을 전담하는 함수를 따로 두고 `print_mutex` 하나로 직렬화했다.

```c
// src/state.c
void	philo_log(t_philo *philo, const char *message)
{
    t_table	*table;
    long	timestamp;

    table = philo->table;
    // pthread_mutex_lock/unlock: 여러 스레드가 동시에 이 구간(critical section)에 들어오지 못하게 잠그는 락(뮤텍스) 
    // lock과 unlock 사이는 한 번에 스레드 하나만 실행됨을 보장한다
    pthread_mutex_lock(&table->print_mutex);
    if (!philo_has_ended(table))
    {
        timestamp = philo_now_ms() - table->start_ms;
        printf("%ld %d %s\n", timestamp, philo->id, message);
    }
    pthread_mutex_unlock(&table->print_mutex);
}
```

출력 직전에 `philo_has_ended()`를 한 번 더 확인하는 이유는, 이미 시뮬레이션이 끝난 뒤에 "is eating" 같은 뒤늦은 상태 로그가 사망 로그 뒤에 찍히는 것을 막기 위해서다. 순서가 뒤집힌 로그는 오작동으로 간주한다. `philo_log_death()`(이후 `philo_try_log_death()`로 이름과 동작이 바뀐다)는 `should_print = !table->ended; table->ended = 1;`으로 사망을 확정한 뒤 로그를 찍는 형태다. 아직 "이미 죽은 시각을 재확인"하는 로직은 없다. 

## 2. `feat(routine): 철학자의 식사/수면/사고 흐름 구현`


```c
// src/routine.c
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

식사하는 철학자 문제의 핵심 트랩. 짝수 id는 오른쪽부터, 홀수 id는 왼쪽부터 포크를 집는다. 모든 철학자가 똑같이 "왼쪽부터" 집으면 원탁 전체가 동시에 왼쪽 포크만 쥔 채 서로의 오른쪽 포크를 기다리는 순환 대기(circular wait)로 완전히 멈출 수 있다. 인접한 두 철학자의 집는 순서를 어긋나게 만들어 이 순환을 원천적으로 끊는다. `philo_routine()`의 첫 형태는 단순하다. 시작 장벽도, N=1 처리도, 식사 중단 처리도 없다. 짝수 철학자가 시작 전 1ms를 더 자는 초기 경합 분산 판단만 있었다.

```c
// void *함수이름(void *arg) 형태는 pthread_create가 스레드로 실행시킬 함수에 요구하는 고정된 시그니처다
// 인자와 반환값을 모두 void*로 받고 돌려줘야 어떤 함수든 스레드로 태울 수 있다
void	*philo_routine(void *arg)
{
    t_philo	*philo;

    philo = (t_philo *)arg;
    if (philo->id % 2 == 0)
        philo_sleep_ms(philo->table, 1);
    while (!philo_has_ended(philo->table))
    {
        eat_once(philo);
        philo_log(philo, "is sleeping");
        philo_sleep_ms(philo->table, philo->table->config.time_to_sleep);
        philo_log(philo, "is thinking");
    }
    return (NULL);
}
```

`eat_once()`는 반환값이 없는 `void` 함수다. 포크를 잡고, 식사 시각을 기록하고, `time_to_eat`만큼 자고, 식사 완료를 기록하고, 포크를 내려놓는 것으로 끝이다. 아직 "이미 다른 철학자가 죽어서 시뮬레이션이 끝난 뒤에도 포크를 얻어 식사를 시작해버리는" 문제가 남아 있다.

## 3. `feat(monitor): 사망과 식사 완료 조건 감시`

각 철학자 스레드가 스스로 자기 자신의 죽음을 판정하게 하지 않고, 별도의 감시 루프(`philo_monitor`)가 메인 스레드에서 모든 철학자를 주기적으로 순회하며 죽음/식사완료 조건을 검사하도록했다. 철학자 스레드 본인은 포크를 쥐고 자느라 바쁠 때가 많아서, "내가 곧 죽는다"를 스스로 실시간으로 감지하게 하려면 로직이 훨씬 복잡해지므로 별도 감시자가 전체 상태를 바깥에서 폴링하는 편이 단순하다는 판단이었다.

```c
// src/monitor.c
void	philo_monitor(t_table *table)
{
    t_philo	*dead;
    long	now;

    while (!philo_has_ended(table))
    {
        now = philo_now_ms();
        pthread_mutex_lock(&table->state_mutex);
        if (all_meals_done(table))
        {
            pthread_mutex_unlock(&table->state_mutex);
            philo_finish(table);
            return ;
        }
        dead = find_dead_philo(table, now);
        pthread_mutex_unlock(&table->state_mutex);
        if (dead != NULL)
        {
            philo_log_death(dead);
            return ;
        }
        usleep(500);
    }
}
```

고쳐야할 두 가지 문제가 존재한다.

1. `while (!philo_has_ended(table))`로 루프를 도는데 `all_meals_done`이 참이 되는 순간과 이 조건을 다시 확인하는 순간 사이에 미묘한 간극이 있다 → "종료 상태와 사망 로그를 원자적으로 확정"하는 방향으로 개선

2. `find_dead_philo`로 찾은 죽음 후보와 `philo_log_death`로 실제 로그를 남기는 시점 사이에 락을 놓았다 다시 잡는데, 그 사이에 해당 철학자가 마침 포크를 얻어 `last_meal_ms`를 갱신했을 수도 있다 → 재확인 로직 추가

## 4. `feat(thread): 철학자 작업 스레드 시작과 종료`

```c
// src/run.c
int	philo_run(t_table *table)
{
    int	i;

    table->start_ms = philo_now_ms();
    i = 0;
    while (i < table->config.number)
    {
        pthread_mutex_lock(&table->state_mutex);
        table->philos[i].last_meal_ms = table->start_ms;
        pthread_mutex_unlock(&table->state_mutex);
        // pthread_create(스레드 저장 위치, 속성, 실행할 함수, 그 함수에 넘길 인자)
        // 성공 시 0을 반환, 이 순간부터 philo_routine이 별도의 실행 흐름으로 동시에 돌기 시작한다
        if (pthread_create(&table->philos[i].thread, NULL, philo_routine, &table->philos[i]) != 0)
        {
            philo_finish(table);
            join_started(table, i);
            return (PHILO_ERR);
        }
        i++;
    }
    philo_monitor(table);
    join_started(table, table->config.number);
    return (PHILO_OK);
}
```

스레드를 하나 만들 때마다 그 철학자의 `last_meal_ms`를 `start_ms`로 찍고 바로 `pthread_create`를 부른다. 문제는 `pthread_create` 자체가 스레드마다 걸리는 시간이 달라서, 먼저 만들어진 철학자와 나중에 만들어진 철학자가 실제로 "동시에 시작"하지 않는다는 점이다. 모두가 같은 `start_ms`를 기준으로 사망 카운트다운을 시작한다고 착각하고 있지만, 실제로 스레드가 뛰기 시작하는 시각은 제각각이다. 이 불공평은 아직 눈에 띄는 버그로 드러나지 않았는데, 인원수가 많아지고 `pthread_create` 지연이 누적되면 뒤에 생성된 철학자일수록 불리해진다 → 이 문제는 조건변수 기반 시작 장벽으로 해결.

`join_started()`도 이 시점에는 반환값이 없는 `void` 함수다. `pthread_join`이 실패할 수 있다는 가능성 자체를 아직 고려하지 않았다 → 이건 `PHILO_UNSAFE` 상태로 다뤄진다.

## 5. `feat(main): 입력부터 자원 정리까지 실행 흐름 연결`

```c
// src/main.c
int	main(int argc, char **argv)
{
    t_config	config;
    t_table		table;

    if (philo_parse_args(argc, argv, &config) != PHILO_OK)
    {
        print_usage();
        return (1);
    }
    if (philo_table_init(&table, &config) != PHILO_OK)
    {
        write(2, "Error: failed to initialize table\n", 34);
        return (1);
    }
    if (philo_run(&table) != PHILO_OK)
    {
        philo_table_destroy(&table);
        write(2, "Error: failed to run philosophers\n", 34);
        return (1);
    }
    philo_table_destroy(&table);
    return (0);
}
```

`philo_parse_args → philo_table_init → philo_run → philo_table_destroy`라는 전체 실행 흐름이 처음으로 끝까지 이어졌다. 이때부터 이 프로그램이 실제로 "식사하는 철학자 문제를 흉내내며 끝까지 실행되는" 완결된 프로그램이 된다. 단, 아직 두 가지는 거칠다: 

1. 에러 메시지를 `write(2, literal, N)`으로 길이를 하드코딩해서 출력

2. `philo_run`이 실패해도 `philo_table_destroy`의 결과 자체는 무시

처음으로 `./philo 5 800 200 200` 같은 명령을 실제로 실행해서 로그가 나오는 걸 눈으로 확인할 수 있게 됐지만 자동화 테스트는 아직 없다.

## 정리

첫 편의 구조는 공유 자원마다 락 하나를 두고, 두 락을 함께 잡을 때는 순서를 고정하고, 판정은 바깥의 감시자가 맡는 형태다. 교착은 홀짝에 따라 포크 순서를 어긋나게 해서 순환 대기 하나만 끊는 방식으로 막았다. 감시 루프가 락을 풀고 다시 잡는 사이의 틈, 스레드마다 다른 출발 시각, 무시되는 실패 반환값은 이 편에서 이미 보였고 뒤의 두 편이 하나씩 고친다.

웹에서는 두 행을 반대 순서로 잠그는 트랜잭션이 이 교착과 같은 구조로 나타난다. 대응 코드는 `appendix/lv2-core/dining-philosophers-consistent-lock-ordering.md`의 웹 섹션에 있다.
