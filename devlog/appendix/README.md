# Appendix

이 프로젝트를 진행하며 마주친 개념 중 devlog 본문에 담기엔 산발적이었던 것들을 독립된 짧은 문서로 정리한다. 각 문서는 그 자체로 완결되게 읽을 수 있다.

레벨: **Lv1** 신입~주니어 / **Lv2** 미들 / **Lv3** 미들 후반~시니어 직전

각 문서는 레벨별로 `lv1/`, `lv2/`, `lv3/` 디렉터리에 위치한다. 이 중 신입/주니어 채용 실무에서 자주 다뤄지는 항목은 `lv2/` 대신 `lv2-core/`에 별도로 모아뒀다(난이도 자체는 Lv2로 동일, 우선순위만 구분).

**현재 상태**: 최대 축소 형태다. 이 프로젝트는 하나의 명확한 주제를 갖기 때문에 devlog 2개 / appendix 2개로 가장 무게가 실리는 지점("왜 정적 상태가 필요한가", "그 상태를 fd로 어떻게 구분하는가")만 골랐다. 버퍼 관리와 시스템 콜 세부는 C 고유의 수명 관리라 웹으로 직접 전이되지 않고, "호출 사이에 남길 상태와 그 상태의 키를 정한다"는 원칙만 남겼다. 공개 API 계약, 정적 라이브러리 빌드와 배포 검증, sanitizer 경로, 명시적 context API 전환은 다루지 않았다. 저장 형태는 devlog 전용 커밋이다. 코드와 같은 브랜치에 두되 `devlog/` 아래 파일만 바꾸는 커밋으로 올린다.

`appendix/`는 `lv1/`과 `lv2-core/`만 작성했고 `lv2/`(비core)와 `lv3/`는 정의조차 하지 않았다.

## 읽는 순서

1. **호출 사이의 상태** `static-state-for-partial-reads-across-calls` → `linked-list-keyed-state-for-multiple-concurrent-handles`

## 항목

- `lv1/static-state-for-partial-reads-across-calls.md`: 함수 호출 사이에 부분 결과를 기억해야 하는 이유 (C 기초)
- `lv2-core/linked-list-keyed-state-for-multiple-concurrent-handles.md`: 키로 찾아가는 연결 리스트로 여러 핸들의 상태를 독립적으로 관리하기 (자료구조)
