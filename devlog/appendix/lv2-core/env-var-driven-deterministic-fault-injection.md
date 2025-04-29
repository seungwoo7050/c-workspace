# 환경변수 기반 결정적 장애 주입(fault injection) 패턴

**Lv2-core / 테스트**

**프로젝트에서** `src/runtime.c`의 모든 시스템 콜/할당 래퍼가 `SMALL_SHELL_TESTING` 빌드에서만 활성화되는 카운터 기반 장애 주입을 공유한다.

```c
static int fail_call(const char *name, unsigned long *calls) {
    (*calls)++;
    text = getenv(name);              // 예: SMALL_SHELL_FAIL_PIPE=3
    target = strtoul(text, &end, 10);
    return target == *calls;          // "이번이 지정된 N번째 호출인가?"
}
```

할당 실패는 한 단계 더 정교해서(`fail_allocation`), "몇 번째 명령의, 어떤 `alloc_scope`에서, 몇 번째 할당을 실패시킬지"까지 3중으로 지정할 수 있다(devlog 02). `tests/faults.sh`/`allocation.sh`는 이 환경변수들을 조합해 셸을 실행하며 각 실패 지점을 순회한다.

**일반적으로** `malloc`이 실제로 `NULL`을 반환하거나 `pipe()`가 실제로 `EMFILE`을 반환하는 상황은, 정상적인 테스트 환경에서 우연히 재현하기 매우 어렵다(메모리를 실제로 고갈시키거나 fd 한도를 실제로 채워야 한다). 그렇다고 이런 실패 경로를 테스트하지 않으면, "에러 처리 코드가 있다"는 것과 "그 에러 처리 코드가 실제로 작동한다"는 것 사이의 간극이 검증되지 않은 채 남는다. 표준적인 해법이 결정적 장애 주입이다. 실제 시스템 콜/라이브러리 함수를 감싸는 얇은 wrapper를 만들고, 테스트 모드에서만 그 wrapper가 "지정된 조건에서 실패를 흉내"내게 한다. Java의 Mockito, Go의 인터페이스 기반 fake, C++의 가상 함수 오버라이드가 전부 같은 목적의 다른 구현이다. 이 프로젝트는 C에서 매크로(`#ifdef SMALL_SHELL_TESTING`)와 환경변수로 같은 역할을 한다.

**모범사례**:  
프로덕션 코드와 테스트 전용 분기가 같은 소스 파일 안에 `#ifdef`로 섞여 있는 이 방식은, 컴파일 타임에 완전히 제거된다는 장점(런타임 비용 0)이 있는 대신 소스 가독성이 떨어진다는 대가가 있다. 더 큰 코드베이스라면 이 wrapper 계층을 별도 컴파일 단위(테스트 전용 double/mock 라이브러리)로 완전히 분리해 링크 시점에 교체하는 방식(링크 시드 목/의존성 주입)을 쓰기도 한다. 이 프로젝트 규모(파일 하나, 함수 몇 개)에서는 `#ifdef` 하나로 충분히 관리 가능한 범위였다.
