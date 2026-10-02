# c-workspace

![Language](https://img.shields.io/badge/language-C99-blue?logo=c&logoColor=white)
![Platform](https://img.shields.io/badge/platform-POSIX-lightgrey)
![Build](https://img.shields.io/badge/build-Make-lightgrey)

42 공통 과정의 C 과제들을 각각 변형해 다시 구현한 저장소입니다. 프로젝트마다 독립된 브랜치를 쓰며, 브랜치끼리 공통 조상이 없는 별개의 루트 히스토리입니다. 이 브랜치(`main`)에는 이 인덱스 문서만 있습니다.

## 프로젝트

| 프로젝트 | 원 과제 | 설명 |
| --- | --- | --- |
| [c-foundation](https://github.com/seungwoo7050/c-workspace/tree/c-foundation) | `libft` | 문자 · 메모리 · 문자열 · fd 출력 · 연결 리스트를 제공하는 정적 라이브러리 |
| [buffered-line-reader](https://github.com/seungwoo7050/c-workspace/tree/buffered-line-reader) | `get_next_line` | 호출 간 상태를 유지하며 fd에서 한 줄씩 읽는 정적 라이브러리 |
| [format-printer](https://github.com/seungwoo7050/c-workspace/tree/format-printer) | `ft_printf` | 포맷 문자열을 해석해 fd 1에 기록하는 `ft_printf` 구현 |
| [stack-sort](https://github.com/seungwoo7050/c-workspace/tree/stack-sort) | `push_swap` | 두 스택과 제한된 연산으로 정수를 정렬하고 독립 checker로 검증 |
| [signal-message-bus](https://github.com/seungwoo7050/c-workspace/tree/signal-message-bus) | `minitalk` | UNIX signal로 문자열을 전달하고 socket 응답 채널로 전달을 확인 |
| [thread-dining](https://github.com/seungwoo7050/c-workspace/tree/thread-dining) | `philo` | pthread와 mutex로 구현한 식사하는 철학자 문제 |
| [small-shell](https://github.com/seungwoo7050/c-workspace/tree/small-shell) | `minishell` | tokenize · parse · expand · redirection · heredoc · pipeline을 수행하는 POSIX 스타일 셸 |

표의 순서는 과제 진행 순서를 따릅니다. 각 브랜치의 `README.md`에 해당 프로젝트의 API, 빌드, 테스트, 구현 범위가 정리되어 있습니다.

## 브랜치 받기

```sh
git clone https://github.com/seungwoo7050/c-workspace.git
cd c-workspace
git switch small-shell # 원하는 프로젝트 브랜치
```

브랜치를 바꾸면 작업 트리 전체가 해당 프로젝트로 교체됩니다. 여러 프로젝트를 동시에 열어두려면 worktree를 쓰는 편이 편합니다.

```sh
git worktree add ../small-shell small-shell
```

## 공통 규약

모든 프로젝트가 같은 규칙을 따릅니다.

- C99, `-Wall -Wextra -Werror` 기준으로 경고 없이 빌드합니다.
- 디렉터리 구조는 `include/`(공개 헤더), `src/`(구현), `tests/`(테스트), `build/`(산출물)로 통일되어 있습니다.
- 빌드와 테스트는 저장소 루트의 `Makefile` 하나로 실행합니다.

```sh
make      # 라이브러리 또는 실행 파일 빌드
make test # 테스트 빌드 후 즉시 실행, 실패 시 0이 아닌 종료 코드
make re   # 전체 재빌드
```

프로젝트에 따라 sanitizer 타깃이 추가로 있습니다. `make test-asan`(`c-foundation`, `buffered-line-reader`, `small-shell`), `make test-ubsan`(`small-shell`), `make test-tsan`(`thread-dining`)입니다.

할당 실패, 시스템 호출 실패 같은 오류 경로는 추측이 아니라 fault injection으로 직접 유발해 검증합니다. 테스트는 성공 · 실패를 종료 코드로 알리므로 자동화에 그대로 쓸 수 있습니다.

## devlog

각 브랜치의 `devlog/`에는 그 프로젝트를 진행하며 정리한 노트가 있습니다.

- `devlog/01-*.md`, `02-*.md`: 해당 과제에서 가장 무게가 실린 주제를 다루는 본문. 예를 들어 `signal-message-bus`의 self-pipe trick, `stack-sort`의 두 스택 radix sort, `small-shell`의 fault injection과 실패 전파.
- `devlog/appendix/`: 본문에 담기엔 산발적이었던 개념을 한 편씩 독립 문서로 정리한 부록. 각 문서는 그 자체로 완결되게 읽을 수 있습니다.

부록은 난이도별 디렉터리로 나뉘며, 신입 · 주니어 채용 실무에서 자주 다뤄지는 항목은 `lv2/` 대신 `lv2-core/`에 모아두었습니다. 읽는 순서와 항목 목록은 각 브랜치의 `devlog/appendix/README.md`에 있습니다.
