# Appendix

이 프로젝트를 진행하며 마주친 개념 중 devlog 본문에 담기엔 산발적이었던 것들을 독립된 짧은 문서로 정리한다. 각 문서는 그 자체로 완결되게 읽을 수 있다.

레벨: **Lv1** 신입~주니어 / **Lv2** 미들 / **Lv3** 미들 후반~시니어 직전

각 문서는 레벨별로 `lv1/`, `lv2/`, `lv3/` 디렉터리에 위치한다. 이 중 신입/주니어 채용 실무에서 자주 다뤄지는 항목은 `lv2/` 대신 `lv2-core/`에 별도로 모아뒀다(난이도 자체는 Lv2로 동일, 우선순위만 구분).

**현재 상태**: 이 프로젝트는 fork/exec/pipe/redirection 같은 POSIX 프로세스 모델과 셸 파싱이 주제라 웹 개발로의 직접 전이가 얇은 "최대 축소" 대상이다. 그래서 devlog를 2개로, appendix를 2개로 줄여 웹에서도 그대로 통하는 "실패 경로를 결정적으로 재현하는 테스트"와 "블랙박스/화이트박스 테스트 계층"만 남겼다. 파서 설계, 프로세스/파이프 FD 생명주기, 종료 코드 관례는 다루지 않는다. `lv1/`과 `lv2-core/`만 작성했고 `lv2/`(비core)와 `lv3/`는 비어 있다. 저장 형태는 devlog 전용 커밋이다. 코드와 같은 브랜치에 두되 `devlog/` 아래 파일만 바꾸는 커밋으로 올린다.

## 읽는 순서

1. **테스트 계층** `blackbox-vs-whitebox-testing` (devlog 01)
2. **실패 경로 테스트** `env-var-driven-deterministic-fault-injection` (devlog 02)

## 항목

- `lv1/blackbox-vs-whitebox-testing.md`: 블랙박스 CLI 테스트와 화이트박스 API 테스트의 차이 (테스트)
- `lv2-core/env-var-driven-deterministic-fault-injection.md`: 환경변수 기반 결정적 장애 주입(fault injection) 패턴 (테스트)
