# Dev Log 01. Two classic 42 grading traps

## 1. `fix(decimal): INT_MIN 크기를 unsigned 범위에서 계산`

```c
// 이전
value = (long)number;
if (value < 0)
    return (ft_write_decimal(ctx, fmt, "-", (unsigned long)(-value)));

// 이후
// 2의 보수 표현에서 음수 쪽 표현 범위가 양수 쪽보다 1 크기 때문에 INT_MIN의 절댓값은 int로 표현이 안되고,
// -value처럼 그대로 음수화하면 표현 범위를 넘어서는 undefined behavior가 된다
// 보통 UB의 경우 컴파일러가 아무 결과나 낼 수 있다는 뜻으로 해석되며 에러 없이 조용히 틀린 값이 나올 수 있어 더 위험하다
if (value < 0)
    // value + 1은 INT_MIN보다 1 커서 안전하게 표현 가능 →
    // 그걸 음수화한 뒤 1을 더해 원래 구하려던 절댓값을 "경계값을 직접 건드리지 않고" 우회해서 얻는다
    return (ft_write_decimal(ctx, fmt, "-", (unsigned long)(-(value + 1)) + 1));
```

`printf("%d", INT_MIN)`(즉 `-2147483648`)은 `ft_printf` 구현이 가장 흔하게 틀리는 지점이다. 부호 있는 정수의 표현 범위는 음수 쪽이 양수 쪽보다 1 크다(2의 보수 표현에서 `INT_MIN`의 절댓값은 `INT_MAX + 1`이라, `int`로는 표현이 안 된다). 그래서 `-value`를 그대로 계산하면, `value`가 `int`와 같은 폭의 `long`으로 캐스팅됐을 때(플랫폼에 따라 `long`이 `int`와 같은 크기일 수 있다) 이 음수화 자체가 표현 범위를 넘는 undefined behavior가 된다.

수정된 코드는 `-value`를 직접 구하는 대신 `(-(value + 1)) + 1`로 우회한다. `value + 1`은 `INT_MIN`보다 1 크므로 안전하게 표현 가능하고, 그 값을 음수화한 뒤 마지막에 1을 더해 원하는 절댓값을 얻는다. 이 계산 전체가 "표현 범위의 경계값 하나를 건드리지 않고" 같은 결과에 도달하는 우회로다.

## 2. `fix(output): 중단된 쓰기 재시도와 요청 크기 제한`

```c
while (length > 0) {
    request = length;
    // SSIZE_MAX: write()가 한 번에 안전하게 처리하도록 보장되는 최대 바이트 수(표준이 정한 상한)
    // 이보다 큰 값을 넘기면 결과가 규정되어 있지 않아서, 매 반복 요청 크기를 이 값으로 잘라낸다
    if (request > (size_t)SSIZE_MAX)
        request = (size_t)SSIZE_MAX; // write()가 SSIZE_MAX보다 큰 요청을 보장하지 않음
    // write(fd, buf, n): fd에 buf의 내용을 최대 n바이트 쓰고, 실제로 쓴 바이트 수를 반환한다
    // 요청한 만큼 전부 쓴다는 보장이 없어 부분 쓰기가 날 수 있고, 실패하면 음수를 반환
    written = write(ctx->fd, buffer, request);
    // errno: 시스템 콜이 실패했을 때 "왜 실패했는지"를 담아두는 전역 변수
    // EINTR: 그 실패 이유 중 하나로, "쓰는 도중 시그널이 와서 중단되었다"는 뜻
    // 진짜 에러가 아니므로 continue로 같은 요청을 다시 시도한다
    if (written < 0 && errno == EINTR)
        continue ; // 시그널 중단은 재시도
    if (written <= 0) { ctx->error = 1; ... }
    ...
}
```

`write(2)`는 세 가지를 보장하지 않는다.

1. 요청한 만큼 전부 쓴다는 것(부분 쓰기 가능),

2. `SSIZE_MAX`보다 큰 `length`를 안전하게 처리한다는 것(POSIX가 결과를 규정하지 않음),

3. 시그널에 의해 중단(`EINTR`)되지 않는다는 것. 

이 함수는 이 세 가지를 전부 방어한다. 남은 만큼 반복해서 쓰고(부분 쓰기 대응), 요청 크기를 `SSIZE_MAX`로 클램프하고, `EINTR`만 실패로 세지 않고 재시도한다.

## 정리

두 사례 다 "라이브러리 함수의 계약을 문서 그대로 읽지 않으면 놓치는" 종류의 함정이다. `-value`가 항상 안전할 거라는 가정, `write()`가 항상 전부 다 쓸 거라는 가정. `ft_printf`처럼 표준 함수를 재구현하는 과제의 진짜 난이도는 "문법을 파싱하는 것"보다 이런 경계 조건을 표준 구현만큼 정확히 재현하는 데 있다.
