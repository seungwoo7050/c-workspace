# Dev Log 01. Sending a string through nothing but signals

## `feat(client): 메시지 바이트를 시그널로 전송`

```c
// pid_t: 프로세스 ID를 담는 전용 정수 타입
// 그냥 int를 써도 무방하지만, "이 값은 PID다"를 타입으로 드러내기 위해 표준에서 따로 정의해둔 이름
static int send_bit(pid_t server_pid, int bit)
{
    signal = MT_ZERO_SIGNAL; // 실제로는 SIGUSR1
    if (bit != 0)
        signal = MT_ONE_SIGNAL; // 실제로는 SIGUSR2
    // kill(pid, sig): 이름과 달리 "죽인다"는 뜻이 아니라, 지정한 프로세스에 시그널 하나를 보낸다는 뜻
    // 성공하면 0, 실패하면 -1을 반환한다
    if (kill(server_pid, signal) == -1)
        return (-1);
    // usleep(ms): ms만큼 현재 실행을 멈춘다 (1s = 1000000ms)
    usleep(150); // 다음 시그널을 서버가 놓치지 않도록 잠깐 대기
    return (0);
}

static int send_byte(pid_t server_pid, unsigned char byte)
{
    shift = 7;
    while (shift >= 0) {
        // (byte >> shift): 비트를 오른쪽으로 shift칸 밀어서 원하는 자리를 맨 끝으로 옮김
        // & 1: 맨 끝 1비트만 남기고 전부 0으로 지움 → 결과는 그 위치의 비트값(0 or 1)
        send_bit(server_pid, (byte >> shift) & 1); // 최상위 비트부터 하나씩
        shift--;
    }
    return (0);
}
```

UNIX 시그널은 "이 시그널이 왔다"는 것 말고는 아무 데이터도 실어 나르지 못한다. `SIGUSR1`/`SIGUSR2` 두 종류뿐이라, 한 번에 표현할 수 있는 정보는 1비트다. 그래서 문자열을 보내려면 각 바이트를 8개의 시그널(비트 하나당 시그널 하나)로 쪼개 순서대로 보내야 한다. `usleep(150)`은 이 초기 버전이 "서버가 이전 시그널을 처리하기 전에 다음 시그널이 도착해 유실되는" 문제를 고정된 지연으로 회피하는 방식이다. 이 방식의 한계(느리고, 그래도 유실 가능성이 있음)는 이후 `feat(protocol): 비트 처리마다 ACK 전송`에서 "고정 지연" 대신 "서버의 확인 응답(ACK)을 받고서야 다음 비트를 보낸다"는 방식으로 대체된다.

## `feat(server): 시그널 비트를 바이트로 조립`

```c
// minitalk.h
...
# include <signal.h>

# define MT_ZERO_SIGNAL SIGUSR1
# define MT_ONE_SIGNAL SIGUSR2
...

// server.c
// volatile: "이 변수는 프로그램 흐름과 무관하게(여기서는 시그널 핸들러가) 언제든 바뀔 수 있다"고 컴파일러에게 알리는 키워드
// 없으면 컴파일러가 값을 레지스터에 캐싱해두고 메모리를 다시 읽지 않는 최적화를 해서 핸들러가 바꾼 값을 메인 코드가 못 볼 수 있다
// sig_atomic_t: 시그널 핸들러와 함께 읽고 써도 값이 "반쪽만 바뀐 상태"로 보이지 않도록 표준이 보장하는 정수 타입
// 시그널 핸들러와 공유하는 전역 변수는 이 타입을 쓰는 것이 관례
static volatile sig_atomic_t g_current_byte;
static volatile sig_atomic_t g_received_bits;

static void handle_bit(int signal, siginfo_t *info, void *context)
{
    unsigned char output;

    // 핸들러는 정해진 시그니처를 따라야해서 context를 받아야 했지만, 여기선 필요 없어서 (void) 처리
    (void)context;
    if (info == NULL || info->si_pit <= 0) // 보낸 프로세스 정보가 비정상이면 무시
        return
    // <<= 1: "왼쪽으로 1비트 밀고 그 결과를 다시 저장" (a <<= 1은 a = a << 1과 같다)
    // 새 비트가 들어올 맨 오른쪽 자리를 0으로 비워두는 효과
    g_current_byte <<= 1;
    if (signal == MT_ONE_SIGNAL)
        // |= 1: "맨 오른쪽 비트를 1로 켠다"
        // 방금 비워둔 자리에 1을 채워 넣는다(0이 왔다면 이 줄을 건너뛰어 비워둔 0이 그대로 남는다).
        g_current |= 1;
    g_received_bits++;
    // 클라이언트가 최상위 비트(7번째)부터 보내므로, 먼저 온 비트일수록 결국 왼쪽에 자리잡는다
    if (g_received_bits == 8)
    {
        output = (unsigned char)g_current_byte;
        // STDOUT_FILENO: 표준 출력을 가리키는 파일 디스크립터 번호(1)를 이름으로 쓴 상수
        // 8비트가 모인 순간 완성된 한 바이트를 바로 출력하고, 다음 바이트를 위해 상태를 초기화
        write(STDOUT_FILENO, &output, 1);
        g_current_byte = 0;
        g_received_bits = 0;
    }
}

static int  install_signal_handlers(void)
{
    // struct sigaction: "이 시그널이 오면 어떻게 처리할지"를 담는 설정 구조체
    struct sigaction action;

    // sa_sigaction: 시그널이 왔을 때 호출할 함수(위의 handle_bit)를 등록하는 필드
    action.sa_sigaction = handle_bit;
    // sa_mask: 핸들러가 실행되는 동안 "일시적으로 막아둘(block) 시그널 목록"
    // sigemptyset으로 목록을 먼저 비운 뒤, sigaddset으로 막을 시그널을 하나씩 추가한다
    sigemptyset(&action.sa_mask);
    // 두 시그널을 모두 막는 이유: handle_bit은 전역 변수를 여러 줄에 걸쳐 수정하므로,
    // 실행 도중 다음 비트의 시그널이 끼어들어 핸들러가 겹쳐 실행되면 상태가 꼬인다
    sigaddset(&action.sa_mask, MT_ZERO_SIGNAL);
    sigaddset(&action.sa_mask, MT_ONE_SIGNAL);
    // SA_SIGINFO: "핸들러가 (signal, info, context) 3개 인자 형태"라는 표시
    // 이 플래그가 있어야 위에서 등록한 sa_sigaction이 사용되고 info->si_pid 등을 받을 수 있다
    action.sa_flags = SA_SIGINFO;
    // sigaction(시그널, 설정, 이전설정저장위치): 실제로 그 시그널에 위 설정을 적용한다
    // 성공하면 0, 실패하면 -1을 반환 (마지막 NULL은 이전 설정을 따로 저장하지 않겠다는 뜻)
    if (sigaction(MT_ZERO_SIGNAL, &action, NULL) == -1)
        return (-1);
    if (sigaction(MT_ONE_SIGNAL, &action, NULL) == -1)
        return (-1);
    return (0);
}

int main(void)
{
    ...
    while (1)
        // pause(): 시그널이 하나 도착할 때까지 프로세스를 잠재운다
        // 실제 작업은 전부 핸들러에서 일어나므로, 메인은 CPU를 쓰지 않고 대기만 한다
        pause();
    return (0);
}
```

받는 쪽은 시그널 핸들러 안에서 비트를 하나씩 받아 `g_current_byte`에 시프트해 쌓다가, 8비트가 모이면(`g_received_bits == 8`) 한 바이트를 완성해 출력한다. 시그널 핸들러가 전역 상태(`g_current_byte`, `g_received_bits`)에 값을 누적한다는 것 자체가, 이 프로젝트 전체를 관통하는 다음 문제(시그널 핸들러 안에서 무엇을 해도 안전한가)로 이어진다.

## 정리

"1비트짜리 채널로 임의 길이의 메시지를 보낸다"는 건 통신 프로토콜을 밑바닥부터 설계하는 축소판이다. 프레이밍(바이트 경계를 어떻게 아는가), 흐름 제어(고정 지연 대 확인 응답), 신뢰성(유실된 비트를 어떻게 알아채는가)이라는, TCP 같은 실제 프로토콜이 다루는 문제들을 훨씬 단순한 형태로 그대로 겪게 된다.
