# Format printer

![Language](https://img.shields.io/badge/language-C99-blue?logo=c&logoColor=white)
![Platform](https://img.shields.io/badge/platform-POSIX-lightgrey)

`format_printer`는 포맷 문자열을 해석해 POSIX 표준 출력 파일 디스크립터 1에 기록하는 C99 정적 라이브러리입니다. 공개 API는 `ft_printf` 하나입니다.

## 지원 범위

| 구분 | 지원 내용 |
| --- | --- |
| 변환 | `%c`, `%s`, `%p`, `%d`, `%i`, `%u`, `%x`, `%X`, `%%` |
| 플래그 | `-`, `0`, `#`, 공백, `+` |
| 필드 | 십진수 너비, `.` 뒤의 십진수 정밀도 |
| 출력 | POSIX `write`를 통한 파일 디스크립터 1 출력 |

성공하면 기록한 바이트 수를 반환하고, 지원하지 않는 형식이나 출력 오류가 발생하면 `-1`을 반환합니다. 길이 지정자, `*`를 이용한 너비, 정밀도, 위치 인자, 부동소수점과 locale 형식은 지원하지 않습니다.

## 빌드

```sh
make
```

정적 라이브러리는 `build/libftprintf.a`로 생성됩니다.

```sh
make re
```

## 사용 예시

```c
#include "ft_printf.h"

int	main(void) {
	  return (ft_printf("value: %#08x, text: %.3s\n", 255u, "hello") < 0);
}
```

```sh
cc -std=c99 -Wall -Wextra -Werror -Iinclude \
	  example.c build/libftprintf.a -o example
./example
```

## 동작 규칙

- 포맷을 먼저 검사하고 측정한 뒤 실제 출력을 수행합니다.
- `%s`의 `NULL`은 `(null)`로 출력합니다.
- `%p`의 NULL 포인터는 `0x0`으로 출력합니다.
- 문자열 정밀도는 NUL 또는 지정한 최대 바이트 수에서 멈춥니다.
- 짧은 `write`는 남은 구간을 이어 쓰고 `EINTR`은 재시도합니다.
- 포맷 문자열과 `%s`가 참조하는 메모리는 호출 중 변경되지 않아야 합니다.

## 테스트

```sh
make test
```

일반 출력과 출력 오류 주입 테스트를 실행합니다. 테스트는 반환값, 출력 바이트 수, 너비, 정밀도, `EINTR`, 짧은 쓰기, `EPIPE`와 0바이트 반환을 확인합니다.

## 제한 사항

- ISO C `printf` 전체와 호환되지 않습니다.
- POSIX `write` 환경을 요구합니다.
- 출력 중 오류가 발생해도 이미 기록한 바이트는 되돌릴 수 없습니다.
- `SIGPIPE` 처리 방식을 변경하지 않습니다.
- 잘못된 가변 인자 타입이나 개수는 검사할 수 없습니다.
- 비차단 출력에서 `EAGAIN`이 발생하면 남은 출력을 보관하지 않습니다.

## 정리

```sh
make clean  # 빌드 및 테스트 산출물 삭제
make fclean # clean 후 정적 라이브러리 삭제
make re     # fclean 후 재빌드
```
