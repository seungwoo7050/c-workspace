# Stack sort

![Language](https://img.shields.io/badge/language-C99-blue?logo=c&logoColor=white)
![Platform](https://img.shields.io/badge/platform-POSIX-lightgrey)

`stack_sort`는 42 `push_swap` 과제를 변형한 C 프로젝트입니다. 두 개의 스택과 제한된 연산만 사용해 정수 입력을 정렬하고, 생성된 명령열을 독립 checker로 검증합니다.

## 프로그램

### `push_swap`

정수 인자를 받아 스택 A를 오름차순으로 정렬하는 명령을 표준 출력에 한 줄씩 기록합니다. 입력은 여러 인자 또는 공백으로 구분한 하나의 문자열로 전달할 수 있습니다.

```sh
./build/bin/push_swap 3 2 1
./build/bin/push_swap "5 -1 4 0 2"
```

중복 값, 정수 범위를 벗어난 값, 숫자가 아닌 입력은 표준 오류에 `Error`를 기록하고 실패합니다.

### `checker`

같은 정수 입력을 받은 뒤 표준 입력의 명령을 실행하고 결과를 출력합니다.

```sh
./build/bin/push_swap 3 2 1 | ./build/bin/checker 3 2 1
printf 'sa\n' | ./build/bin/checker 2 1
```

정렬되면 `OK`, 정렬되지 않으면 `KO`를 출력합니다. 지원하는 명령은 `sa`, `sb`, `ss`, `pa`, `pb`, `ra`, `rb`, `rr`, `rra`, `rrb`, `rrr`입니다.

## 빌드

저장소 루트에서 실행합니다.

```sh
make
```

`push_swap`과 `checker`는 `build/bin/`에 생성됩니다. 중간 오브젝트는 `build/obj/`, fault injection 오브젝트와 실행 파일은 `build/fault/`, invariant 테스트는 `build/test/`에 생성됩니다.

## 테스트

```sh
make test
```

다음 검증을 순서대로 실행합니다.

- 스택 연산 invariant 테스트
- 입력 파싱 경계값과 checker 명령 테스트
- 작은 입력과 무작위 입력의 정렬 결과
- 100개와 500개 입력의 명령 수 제한
- malloc/read 실패 주입과 메모리 정리

## 정리

```sh
make clean  # build/ 및 Python 테스트 캐시 삭제
make fclean # clean 후 push_swap과 checker 삭제
make re     # fclean 후 전체 재빌드
```

빌드 산출물과 테스트 캐시는 저장소에 포함하지 않습니다.
