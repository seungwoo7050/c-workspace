# Dev Log 02. Making failure paths testable: fault injection and end-to-end propagation

## 1. `refactor(runtime): 실행 경로의 동적 할당 래퍼 통합`

### 할당과 시스템 콜을 래퍼 하나로 모은다

```c
void *shell_calloc(size_t count, size_t size) {
    if (size != 0 && count > SIZE_MAX / size) {   // 곱하기 전에 오버플로를 나눗셈으로 검사
        errno = ENOMEM;
        return NULL;
    }
    return calloc(count, size);
}
```

이전에는 할당이 실패하면 메시지를 찍고 `exit(1)`하는 `sh_xcalloc` 계열이었다. 이 리팩터로 "실패하면 죽는" 버전이 사라지고, 실패를 어떻게 처리할지는 호출한 쪽의 몫이 된다. 실제 크기 문제와 이후 주입되는 실패가 같은 방식(`errno = ENOMEM`, `NULL` 반환)으로 보이므로 호출부는 한 가지 방식으로만 처리하면 된다.

## 2. `test(exec): pipe/fork/wait 실패 회귀 검증` / `test(io): read/write와 heredoc 입력 실패 검증`

### 테스트 빌드에서만 켜지는 결정적 장애 주입

```c
int shell_fflush(FILE *stream) {
#ifdef SMALL_SHELL_TESTING
    static unsigned long calls;
    if (fail_call("SMALL_SHELL_FAIL_FFLUSH", &calls)) {   // 환경변수가 N이면 "N번째 호출"을 실패시킨다
        errno = ENOSPC;
        return EOF;
    }
#endif
    return fflush(stream);
}
```

```sh
# 장애가 주입돼도 셸은 죽지 않고 다음 줄까지 계속 동작해야 한다
env "$variable=$call" "$TIMEOUT" 5 "$BIN" < input > out
[ "$status" -eq 0 ] || fail "$name"
```

`malloc`이 실제로 `NULL`을 주거나 `dup2`가 실제로 실패하는 상황은 일반 테스트 환경에서 우연히 재현되지 않는다. 그렇다고 테스트하지 않으면 "에러 처리 코드가 있다"와 "그 코드가 실제로 동작한다" 사이의 간극이 검증되지 않는다. 래퍼가 테스트 모드에서 지정된 N번째 호출을 실패시키고, 테스트 스크립트는 각 실패 지점을 순회하며 셸이 죽지 않고 올바른 종료 상태와 출력을 내는지 확인한다.

## 3. `fix(memory/heredoc/input/io): 실패가 끝까지 올라가는지 확인하며 고친 것들`

주입으로 실패 지점을 하나씩 밟으면서 드러난 문제는 다음과 같다.

- **NULL 반환을 의미별로 구분한다**: 할당 실패와 EOF를 같은 `NULL`로 돌려주면 호출부가 구분하지 못한다. 입력 계층은 `fgetc` 대신 `read()`를 직접 쓰고 out-parameter(`failed`)로 EOF와 입력 실패를 나눈다.
- **실패 후 입력 경계를 복구한다**: heredoc 하나를 준비하다 실패해도 그 구분자 줄이 나올 때까지 입력을 읽어 버리는 `discard_heredoc()`이 없으면, 본문 줄들이 다음 명령으로 실행된다.
- **출력 실패도 상태로 전파한다**: `printf` 대신 `write()` 래퍼를 써서, 디스크가 꽉 찬 상태의 `echo`/`env`가 종료 상태 1로 끝나게 했다.
- **실패해도 옛 상태를 지킨다**: 환경 변수 갱신이 중간에 실패하면 이전 상태가 그대로 남아야 한다.

## 정리

실패 경로는 "처리 코드가 있는가"가 아니라 "주입해서 실제로 밟아 봤는가"로 검증해야 한다. 그러려면 실패할 수 있는 호출을 한 곳에 모아 테스트 모드에서 결정적으로 실패시킬 수 있어야 하고, 각 계층은 실패를 삼키지 않고 의미를 잃지 않은 채 위로 올려야 한다.
