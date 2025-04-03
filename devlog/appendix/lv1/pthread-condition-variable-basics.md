# pthread 조건변수 기본 API (wait/broadcast, spurious wakeup 방어)

**Lv1 / 동시성**

**프로젝트에서** `src/routine.c:10-37`의 `wait_for_start()`가 `pthread_cond_wait`/`pthread_cond_broadcast`의 기본 사용 패턴을 보여준다.

```c
// src/routine.c:16-33
pthread_mutex_lock(&table->state_mutex);
table->ready_count++;
pthread_cond_broadcast(&table->start_cond);
while (!table->start_released)
{
    if (pthread_cond_wait(&table->start_cond, &table->state_mutex) != 0)
    {
        table->run_error = 1;
        table->ended = 1;
        table->start_released = 1;
        pthread_cond_broadcast(&table->start_cond);
    }
}
```

**일반적으로** 조건변수(condition variable)는 그 자체로는 아무 상태도 갖지 않는다. "누군가 나에게 신호를 보낼 때까지 재워달라"는 대기소일 뿐이고, 실제 조건(여기서는 `start_released`)은 항상 별도의 뮤텍스로 보호되는 평범한 변수로 따로 둬야 한다. 그래서 `pthread_cond_wait(cond, mutex)`는 반드시 그 뮤텍스를 이미 잠근 상태에서 불러야 하고, 내부적으로 "뮤텍스를 풀고 잠들었다가, 깨어나면 다시 그 뮤텍스를 잠근 채로 반환"하는 원자적 동작을 한다.

`while (!조건)`으로 감싸는 것이 핵심 관용구다. `pthread_cond_wait`는 아무도 `signal`/`broadcast`를 부르지 않았는데도 운영체제 스케줄러 사정으로 그냥 깨어날 수 있다(spurious wakeup, 허위 기상). `if (!조건) wait()`처럼 한 번만 확인하면 이 허위 기상에 그대로 속아 조건이 아직 안 됐는데 통과해버린다. `while` 루프는 깨어날 때마다 조건을 다시 확인해서 이 문제를 근본적으로 없앤다(POSIX 표준 자체가 이 관용구를 전제로 설계되어 있다).

**모범사례**:  
`pthread_cond_broadcast`는 대기 중인 스레드 전부를 깨우고, `pthread_cond_signal`은 그중 하나만 깨운다. 이 프로젝트는 항상 `broadcast`만 쓴다. "준비됐다"는 신호를 부모가 기다리고, "전원 출발"이라는 신호를 자식들이 다 같이 기다리는 다대일/일대다 구조라 `signal`로 하나씩 깨워서는 의미가 없기 때문이다. 다수의 대기자 중 정확히 하나만 일을 처리해야 하는 생산자-소비자 큐 같은 구조였다면 `signal`이 더 맞는 선택일 수 있다.
