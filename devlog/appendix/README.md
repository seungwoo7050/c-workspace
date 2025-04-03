# Appendix

이 프로젝트를 진행하며 마주친 개념 중 devlog 본문에 담기엔 산발적이었던 것들을 독립된 짧은 문서로 정리한다. 각 문서는 그 자체로 완결되게 읽을 수 있다.

레벨: **Lv1** 신입~주니어 / **Lv2** 미들 / **Lv3** 미들 후반~시니어 직전

각 문서는 레벨별로 `lv1/`, `lv2/`, `lv3/` 디렉터리에 위치한다. 이 중 신입/주니어 채용 실무에서 자주 다뤄지는 항목은 `lv2/` 대신 `lv2-core/`에 별도로 모아뒀다(난이도 자체는 Lv2로 동일, 우선순위만 구분).

**현재 상태**: 최대 축소 형태다. 이 프로젝트는 하나의 명확한 주제(제한된 연산으로 정렬)라 devlog 2개 / appendix 2개로, "연산 제약이 알고리즘 선택을 어떻게 바꾸는가"와 "검증 코드를 프로덕션 코드와 어떻게 분리하는가"만 골랐다. 스택 연산 구현과 입력 파싱은 웹으로 전이되지 않아 다루지 않았고, 3~5개 입력의 전용 정렬, 명령 수 상한 검증, 할당 실패와 읽기 실패 처리도 다루지 않았다. 저장 형태는 devlog 전용 커밋이다. 코드와 같은 브랜치에 두되 `devlog/` 아래 파일만 바꾸는 커밋으로 올린다.

`appendix/`는 `lv1/`과 `lv2-core/`만 작성했고 `lv2/`(비core)와 `lv3/`는 정의조차 하지 않았다.

## 읽는 순서

1. **제약이 정하는 알고리즘** `dense-rank-conversion-before-radix-sort`
2. **독립 검증** `independent-verifier-does-not-share-production-logic`

## 항목

- `lv2-core/dense-rank-conversion-before-radix-sort.md`: 기수 정렬 전에 값을 조밀한 상대 순위로 바꿔야 하는 이유 (알고리즘)
- `lv1/independent-verifier-does-not-share-production-logic.md`: 검증 코드가 프로덕션 로직을 공유하면 안 되는 이유 (테스트)
