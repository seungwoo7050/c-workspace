# Dev Log 02. Getting unsafe work out of the signal handler

## `refactor(server): signal 처리를 self-pipe event loop로 제한`

```c
typedef struct s_bit_event { pid_t sender; int signal; } t_bit_event;
// 배열 크기로 (조건식) * 2 - 1을 쓰면 조건이 참이면 크기 1, 거짓이면 크기 -1
// 즉 실행 없이 "컴파일이 여부" 자체로 조건을 검사하게 만드는 스킬
typedef char t_event_must_fit_pipe_buf[(sizeof(t_bit_event) <= PIPE_BUF) * 2 - 1]; // 컴파일 타임 assert

// 시그널 핸들러로 등록되는 함수는 이 정해진 형태(int, siginfo_t*, void*)를 따라야 한다
// siginfo_t *info: 시그널에 대한 부가 정보(누가 보냈는지 등)를 담은 구조체 포인터
// void *context: 시그널이 발생한 시점의 실행 문맥 정보(대부분의 경우 직접 다룰 일 없음)
static void handle_bit(int signal, siginfo_t *info, void *context)
{
    t_bit_event event;
    ...
    event.signal = signal;
    // 시그널 핸들러 안에서는 표준이 안전하다고 보장한 극히 일부 함수만 호줄할 수 있고 write()는 그 중 하나다
    // 그래서 핸들러는 값을 직접 처리하지 않고 파이프에 써서 넘기기만 한다
    write(g_event_pipe[1], &event, sizeof(event)); // 핸들러는 딱 이것만 한다
}
```

이 커밋 이전에는 시그널 핸들러(`process_bit`)가 직접 비트를 조립하고, 바이트가 완성되면 출력하고, 클라이언트에게 보낼 ACK 응답까지 그 안에서 준비했다. 이 커밋은 그 전부를 핸들러 밖으로 뺀다. 핸들러가 하는 일은 이제 "무슨 시그널이 왔고 누가 보냈는지"를 작은 구조체(`t_bit_event`)에 담아 파이프에 `write()`하는 것뿐이다. 실제 비트 조립, 바이트 출력, ACK 전송은 메인 이벤트 루프가 그 파이프를 읽어서 처리한다(이걸 self-pipe 트릭이라 부른다).

`t_event_must_fit_pipe_buf`는 실행되지 않는 컴파일 타임 검사다. 배열 크기가 음수면 컴파일 자체가 실패하므로, `sizeof(t_bit_event) <= PIPE_BUF`가 거짓이면 빌드가 깨진다. 이건 "PIPE_BUF 이하 크기의 쓰기는 원자적"이라는 보장이 실제로 이 구조체 크기에서 성립하는지를 코드 자체가 스스로 확인하게 만든 것이다.

## 정리

"시그널 핸들러 안에서 무엇을 해도 되는가"라는 질문 하나가 이 프로젝트의 서버 쪽 설계 전체를 결정한다. 핸들러 안에서 안전하게 부를 수 있는 함수(async-signal-safe function)는 표준이 정한 아주 좁은 목록뿐이고 `write()`는 그 목록에 들어있다. 그래서 "핸들러는 파이프에 쓰기만 하고, 나머지는 전부 핸들러 밖(평범한 메인 루프)으로 미룬다"는 게 신호 처리 코드의 정석적인 구조다.
