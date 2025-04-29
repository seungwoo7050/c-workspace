# Dev Log 01. Black-box regression tests and an error contract that silently lied

## 1. `test(smoke): 주요 셸 명령 흐름 검증`

### 실행 파일을 밖에서 찌르는 회귀 테스트

```sh
# 케이스 실행기: (이름, 셸에 넣어줄 입력, 기대 출력, 기대 종료 상태)
run_case() {
    printf '%s' "$input" > "$TMP/$name.in"
    "$TIMEOUT" 5 "$BIN" < "$TMP/$name.in" > "$TMP/$name.out" 2> "$TMP/$name.err"
    status=$?
    [ "$status" -eq "$expected_status" ] || fail "$name"
    cmp -s "$TMP/$name.expected" "$TMP/$name.out" || fail "$name"
}
```

`tests/smoke.sh`는 컴파일된 셸 바이너리에 표준 입력으로 명령을 흘려보내고 **출력과 종료 코드만** 비교한다. 내부 함수를 전혀 모른 채 "밖에서 보이는 동작"만 검증하는 블랙박스 방식이다. 케이스에는 종료 코드 관례(없는 명령 127, 실행 권한 없음 126, 시그널 종료 128+n, 문법 오류 258)와 heredoc 인용 규칙, 리다이렉션 우선순위가 들어 있다. 테스트마다 5초 타임아웃을 걸어, 셸이 멈춰도 테스트 전체가 멈추지 않게 했다.

## 2. `fix(parser): 오류 출력 포인터 없이도 구문 실패 반환`

```c
int shell_parse_line(const char *line, t_sequence *sequence, char **error) {
    internal_error = NULL;
    error_slot = error ? error : &internal_error;   // 호출자가 NULL을 줘도 내부에서는 항상 슬롯을 쓴다
    tokens = tokenize_line(line, error_slot);
    if (*error_slot) { free(internal_error); return 1; }
    ...
}
```

이전 버전은 호출자가 `error`에 `NULL`을 넘기면(실패 여부만 알고 싶을 때) `if (error && *error) return 1;`이 항상 거짓이 되어 **문법 오류를 보고도 성공(0)을 반환**했다. `echo |` 같은 명백한 오류가 조용히 "성공"이 되는 버그다. 수정은 메시지는 호출자가 원하지 않으면 버리되, "실패했다"는 사실은 항상 정확히 반환하는 것이다.

이 버그는 CLI 출력만으로는 구분하기 어려워서, 실행 파일을 거치지 않고 `shell_parse_line()`을 직접 링크해 호출하는 `tests/parser_api.c`(화이트박스)를 추가해 `error` 없이 호출하는 경우까지 고정했다. `make test`는 이 시점부터 블랙박스와 화이트박스를 함께 돌린다.

## 정리

회귀 테스트의 기본은 사용자가 실제로 보는 동작을 블랙박스로 고정하는 것이다. 여기에 블랙박스가 놓치는 내부 계약(선택적 출력 매개변수가 NULL일 때의 동작)만 화이트박스로 보강한다. "오류 정보를 받을지 여부"가 "오류가 났는지 여부"를 바꾸면 안 된다는 것이 이 devlog의 핵심 교훈이다.
