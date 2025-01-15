# C Foundation

![Language](https://img.shields.io/badge/language-C99-blue?logo=c&logoColor=white)
![Build](https://img.shields.io/badge/build-Make-lightgrey)

`c-foundation`은 C 표준 라이브러리의 기본 기능을 직접 구현해 보는 42 libft 과제를 변형한 정적 라이브러리입니다. 문자 검사, 메모리 조작, 문자열 처리, 파일 디스크립터 출력, 단일 연결 리스트 API를 제공합니다.

구현은 C99와 다음 컴파일 옵션을 기준으로 검증합니다.

```text
-std=c99 -Wall -Wextra -Werror -pedantic
```

## 주요 기능

### 문자 처리

- `ft_isalpha`, `ft_isdigit`, `ft_isalnum`
- `ft_isascii`, `ft_isprint`
- `ft_toupper`, `ft_tolower`

### 메모리 처리

- `ft_memset`, `ft_bzero`
- `ft_memcpy`, `ft_memmove`
- `ft_memchr`, `ft_memcmp`

### 문자열 처리

- 길이, 복사, 연결: `ft_strlen`, `ft_strlcpy`, `ft_strlcat`
- 검색 및 비교: `ft_strchr`, `ft_strrchr`, `ft_strncmp`, `ft_strnstr`
- 생성 및 변환: `ft_strdup`, `ft_substr`, `ft_strjoin`, `ft_strtrim`
- 분리 및 순회: `ft_split`, `ft_strmapi`, `ft_striteri`
- 숫자 변환: `ft_atoi`, `ft_itoa`

### 메모리 할당

- `ft_calloc`
- 동적 문자열을 생성하는 문자열 함수

동적 할당이 여러 단계로 이루어지는 함수는 중간 할당이 실패하면 이미 확보한 자원을 정리한 뒤 `NULL`을 반환하도록 구현되어 있습니다.

### 파일 디스크립터 출력

- `ft_putchar_fd`
- `ft_putstr_fd`
- `ft_putendl_fd`
- `ft_putnbr_fd`

### 단일 연결 리스트

`t_list`를 기반으로 노드 생성, 삽입, 삭제, 순회, 변환 기능을 제공합니다.

- 생성 및 삽입: `ft_lstnew`, `ft_lstadd_front`, `ft_lstadd_back`
- 조회: `ft_lstsize`, `ft_lstlast`
- 삭제: `ft_lstdelone`, `ft_lstclear`
- 순회 및 변환: `ft_lstiter`, `ft_lstmap`

## 빌드

저장소 루트에서 실행합니다.

```sh
make
```

생성 결과:

```text
build/libft.a
```

빌드 산출물을 삭제하려면 다음 명령을 사용합니다.

```sh
make clean
```

라이브러리를 삭제한 뒤 처음부터 다시 빌드하려면 다음 명령을 사용합니다.

```sh
make re
```

## 사용 예시

```c
#include "libft.h"

#include <stdlib.h>

int	main(void) {
	  char	*text;
	  t_list	*node;

    text = ft_strdup("hello, libft");
    if (text == NULL)
        return (1);
    node = ft_lstnew(text);
    if (node == NULL) {
        free(text);
        return (1);
    }
    ft_lstclear(&node, free);
    return (0);
}
```

직접 작성한 프로그램을 라이브러리와 함께 빌드하려면 `include` 경로와 정적 라이브러리를 지정합니다.

```sh
cc -std=c99 -Wall -Wextra -Werror -pedantic \
    -Iinclude example.c build/libft.a -o example
```

## 테스트

`make test`는 테스트 프로그램을 빌드한 뒤 즉시 실행합니다.

```sh
make test
```

성공하면 다음과 같은 결과가 출력됩니다.

```text
4095 checks passed
libft tests: PASS
```

테스트에 AddressSanitizer와 UndefinedBehaviorSanitizer를 함께 사용하려면 다음 명령을 실행합니다.

```sh
make test-asan
```

두 테스트 모두 하나라도 검사에 실패하거나 sanitizer 오류가 발생하면 0이 아닌 종료 코드로 종료되므로, 성공 여부를 자동으로 판단할 수 있습니다.

테스트 코드는 다음 범위를 포함합니다.

- 문자 및 문자열 경계값
- 메모리 복사, 이동, 검색 및 비교
- 문자열 생성과 동적 할당 실패 처리
- `ft_split`과 리스트 함수의 정리 동작
- 파일 디스크립터 출력
- 연결 리스트의 생성, 삽입, 삭제, 순회 및 변환

## 프로젝트 구조

```sh
c-foundation/
├── include/
│   └── libft.h          # 공개 헤더
├── src/
│   ├── alloc/           # 메모리 할당
│   ├── char/            # 문자 처리
│   ├── convert/         # 숫자 및 문자열 변환
│   ├── io/              # 파일 디스크립터 출력
│   ├── list/            # 단일 연결 리스트
│   ├── memory/          # 메모리 조작
│   └── string/          # 문자열 처리
├── tests/               # 단위 테스트
├── build/               # 빌드 산출물
├── Makefile
└── README.md
```

## 구현 범위

이 라이브러리는 바이트와 NUL 종료 문자열을 중심으로 동작합니다. Unicode 또는 locale 기반 문자열 해석, 스레드 동기화, 이중 연결 리스트, 순환 리스트 관리는 제공하지 않습니다.

호출자는 각 함수의 인자 유효성, 버퍼 크기, 반환된 동적 메모리의 해제를 관리해야 합니다. 리스트 노드의 `content`를 해제할 때는 해당 데이터에 맞는 `del` 함수를 `ft_lstdelone` 또는 `ft_lstclear`에 전달해야 합니다.
