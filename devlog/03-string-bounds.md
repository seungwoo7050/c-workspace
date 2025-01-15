# Dev Log 03. Bounded string functions: leaving room for the terminator

`ft_strlcpy`/`ft_strlcat`은 표준 `strcpy`/`strcat`이 아니라 BSD의 `strlcpy`/`strlcat`(용량 상한을 받는 버전)을 재구현한다. 이 문서는 그 "용량 상한"이 실제로 무엇을 막아주는지에 집중한다.

## 항상 널 종료 자리 한 칸을 남겨둔다

```c
// src/string/ft_string_bounds.c
size_t ft_strlcpy(char *dest, const char *src, size_t capacity) {
    src_length = ft_strlen(src);
    if (capacity == 0)
        return (src_length);
    idx = 0;
    while (src[idx] != '\0' && idx + 1 < capacity) { // capacity가 아니라 capacity - 1까지만
        dest[idx] = src[idx];
        ++idx;
    }
    // '\0'(NUL 문자)은 문자열의 끝을 표시하는 특수 문자
    // C 문자열은 이 문자가 나오는 지점까지를 문자열로 취급한다
    dest[idx] = '\0';
    return (src_length);
}
```

루프 조건이 `idx < capacity`가 아니라 `idx + 1 < capacity`다. 이 한 칸 차이가 핵심이다. `capacity`를 꽉 채울 때까지 복사하면 널 종료 문자를 쓸 자리가 안 남아서, `dest`가 문자열로서 끝을 알 수 없는 상태(버퍼 오버런으로 이어지는 전형적인 원인)가 된다. 항상 한 칸을 비워두고 그 자리에 `'\0'`을 쓰기 때문에, `dest`에 `capacity`바이트를 넘겨받았어도 그 안에서 항상 유효한(널 종료된) 문자열만 만들어진다.

반환값은 실제로 복사한 길이가 아니라 "`capacity` 제약이 없었다면 필요했을 전체 길이"(`src_length`)를 반환한다. 그래서 호출자는 `if (ft_strlcpy(dest, src, capacity) >= capacity)`처럼 "잘렸는지 여부"를 반환값 하나로 판단할 수 있다.

## `ft_strlcat`의 "이미 깨진 입력" 방어

```c
if (dest_length == capacity)
    return (capacity + src_length);
```

`dest`가 이미 `capacity` 안에 널 종료 없이 꽉 차 있는 비정상 상태(호출자가 계약을 어긴 입력)를 만나면, 어디까지가 원래 문자열인지 알 방법이 없다. 이 함수는 그럴 때 아무것도 이어붙이지 않고 "실패했다"는 뜻으로 큰 값을 반환한다. 알 수 없는 상태를 추측해서 처리하려 들지 않고, 계약 위반 자체를 그대로 반환값으로 드러내는 선택이다.

> 정상적인 상황이라면 `dest` 안 어디엔가 널 문자(`\0`)가 있어야 하므로, 기존 문자열의 길이(`dest_length`)는 항상 전체 상자 크기(`capacity`)보다 작아야 한다 (dest_length < capacity). 하지만 `dest_length == capacity`가 되었다는 것은 `capacity` 크기(예: 10칸)를 전부 훑을 때까지 널 문자(`\0`)를 단 하나도 만나지 못했다는 뜻이다.
> 즉, `dest` 상자가 널 종료도 없이 글자로 꽉 차 있어서 "어디가 기존 문자열의 끝인지 알 수 없는 깨진 상태"임을 의미한다.

## 정리

"버퍼 크기를 넘겨받는 함수는 항상 널 종료 자리를 계산에 포함해야 한다"는 건 C 표준 문자열 함수(`strcpy`, `sprintf` 등)가 반복적으로 실무 취약점(CWE-120류 버퍼 오버플로)의 원인이 됐던 이유이기도 하다. `ft_strlcpy`/`ft_strlcat`을 직접 구현해보는 과제의 핵심은 "왜 `strcpy` 대신 `strlcpy`가 더 안전한지"를 코드 레벨에서 체감하는 것이다.
