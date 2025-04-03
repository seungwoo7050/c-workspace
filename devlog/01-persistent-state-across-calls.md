# Dev Log 01. Remembering what was already read

## 1. `feat(reader): 파일 끝의 마지막 줄 반환`

```c
// typedef struct name { ... } alias; 형태로 구조체 정의와 동시에 그 구조체를 가리킬 별칭(t_reader)을 만든다
typedef struct s_reader { int fd; char *bytes; size_t length; size_t capacity; } t_reader;
// 함수 안에서 static으로 선언한 변수는 함수 호출이 끝나도 값이 사라지지않고 다음 호출까지 유지된다
// {-1, NULL, 0, 0}처럼 중괄호로 구조체 필드를 선언 순서대로 한 번에 초기화할 수 있다
static t_reader g_reader = {-1, NULL, 0, 0}; // 함수 호출 사이에 값이 유지되는 정적 변수

char *get_next_line(int fd)
{
    ...
    // read(fd, buffer, n): fd에서 최대 n바이트를 buffer로 읽어 넣고 실제로 읽은 바이트 수를 반환한다
    // 더 읽을 게 없으면 0, 에러면 음수를 반환한다
    read_size = read(fd, buffer, (size_t)BUFFER_SIZE);
    while (read_size > 0) {
        append_bytes(buffer, (size_t)read_size); // 이번 읽은 만큼을 누적 버퍼 뒤에 붙임
        read_size = read(fd, buffer, (size_t)BUFFER_SIZE);
    }
    ...
}
```

`get_next_line`이 다른 함수와 근본적으로 다른 점은 "한 번의 호출로 한 줄을 반환해야 하는데, 파일에서 한 번 `read()`한 만큼이 딱 한 줄과 일치할 보장이 전혀 없다"는 것이다. `read()`는 개행 문자를 기준으로 끊어주지 않고 그냥 다음 `BUFFER_SIZE`바이트를 돌려줄 뿐이다. 그래서 이 함수는 호출이 끝나도 사라지지 않는 상태(`static t_reader g_reader`)를 갖고, 매 호출마다 "지금까지 읽었지만 아직 반환하지 않은 나머지"를 그 상태에 누적해둔다. 함수가 무상태(stateless)일 수 없다는 게 이 과제의 핵심 전제다.

## 2. `feat(state): 디스크립터별 읽기 상태 분리`

```c
// 이전: 전역 변수 하나 → 파일 디스크립터 하나만 추적
static t_reader g_reader;

// 이후: 연결 리스트 → 여러 fd를 동시에, 서로 간섭 없이 추적
// 구조체 본문 안에서는 아직 typedef 이름(t_reader)을 쓸 수 없어서, 자기 자신을 가리키는 필드는 typedef 이름 대신 struct 태그(struct s_reader)로 써야 한다
typedef struct s_reader { ...; struct s_reader *next; } t_reader;
// 함수 밖에서 선언한 static 전역 변수는 이 소스 파일 밖에서는 안보이게 범위를 제한한다는 뜻
// 위 g_reader의 static과 키워드는 같지만 의미가 다르다. 여기서는 "유지"가 아니라 "접근 범위 제한"의 의미를 갖는다
static t_reader *g_readers;

static t_reader *find_reader(int fd) {
    reader = g_readers;
    while (reader != NULL && reader->fd != fd)
        reader = reader->next;
    return (reader);
}
```

초기 버전은 전역 상태가 딱 하나라서, `get_next_line(fd_a)`로 절반쯤 읽다가 `get_next_line(fd_b)`를 호출하면 `fd_a`의 상태가 통째로 리셋됐다. 파일 하나만 순서대로 끝까지 읽는 경우엔 문제가 없지만, 여러 파일을 번갈아 읽어야 하는 순간 바로 깨진다. 이 커밋은 전역 변수 하나를 fd로 찾아가는 연결 리스트로 바꿔서, 각 fd가 자기만의 독립된 상태를 갖게 한다. "이 함수가 기억해야 하는 상태의 키가 무엇인가"(여기서는 파일 디스크립터)를 다시 짚은 것이다.

### C언어 static 키워드와 전역 변수 형태 정리

```c
// 1. 스코프 외부 + static (파일 전용 전역 변수)
static t_reader *g_readers;
// [수명] 프로그램 시작부터 종료까지 메모리에 상주
// [범위] 현재 .c 파일 내부의 모든 함수에서 접근 가능
// [특징] 다른 .c 파일에서는 이 변수의 존재를 알 수 없음 (캡슐화, 이름 충돌 방지)

// 2. 스코프 외부 + 일반 선언 (전체 공개 전역 변수)
int g_global_score;
// [수명] 프로그램 시작부터 종료까지 메모리에 상주
// [범위] 프로젝트 내의 모든 .c 파일에서 접근 가능 (extern 키워드로 불러옴)
// [특징] 어디서든 접근 가능하여 편리하지만, 다른 파일과 변수명이 겹치면 충돌 발생

void example_function(void)
{
    // 3. 스코프 내부 + static (지역 static 변수)
    static int call_count = 0;
    // [수명] 함수가 종료되어도 사라지지 않고 값이 유지됨 (메모리에 상주)
    // [범위] 오직 이 함수(example_function) 내부에서만 접근 가능
    // [특징] 함수 호출 횟수 세기, 상태 유지 등에 사용되며 외부 접근 불가

    call_count++;
}
```

## 정리

"함수 호출 사이에 상태를 유지해야 한다"는 요구가 생기면, 그 다음 질문은 항상 "그 상태를 무엇으로 구분(key)할 것인가"다. 처음엔 "상태가 하나만 있으면 되겠지"로 시작했다가, 실제 사용 패턴(여러 fd를 동시에 다룸)을 마주치면 그 키를 다시 설계해야 한다는 걸 이 두 커밋이 보여준다.
