# Appendix

이 프로젝트를 진행하며 마주친 개념 중 devlog 본문에 담기엔 산발적이었던 것들을 독립된 짧은 문서로 정리한다. 각 문서는 그 자체로 완결되게 읽을 수 있다.

레벨: **Lv1** 신입~주니어 / **Lv2** 미들 / **Lv3** 미들 후반~시니어 직전

각 문서는 레벨별로 `lv1/`, `lv2/`, `lv3/` 디렉터리에 위치한다. 이 중 신입/주니어 채용 실무에서 자주 다뤄지는 항목은 `lv2/` 대신 `lv2-core/`에 별도로 모아뒀다(난이도 자체는 Lv2로 동일, 우선순위만 구분).

**현재 상태**: `c-foundation`의 `devlog`는 libft 전체(11개 기능군)를 다루는 대신, "프로그래밍 진입" 관점에서 가장 무게가 실리는 세 기능군(메모리, 연결 리스트, 경계 있는 문자열 함수)만 골라 다뤘다. 

`appendix/`는 `lv1/`과 `lv2-core/`만 작성했고 `lv2/`(비core)와 `lv3/`는 정의조차 하지 않았다.

## 읽는 순서 추천

1. **메모리** `overlap-safe-memory-move-direction-detection`
2. **연결 리스트** `free-after-reading-next-pointer` → `rollback-on-partial-failure-in-list-transform`
3. **경계 있는 문자열** `capacity-bounded-copy-reserves-terminator-slot`
4. **공통 함정** `unsigned-char-cast-for-byte-comparisons`

## 메모리

- `lv2-core/overlap-safe-memory-move-direction-detection.md`: 겹치는 메모리 영역을 안전하게 옮기기 위한 방향 탐지 (메모리)

## 연결 리스트

- `lv2-core/free-after-reading-next-pointer.md`: 해제 전에 다음 포인터를 먼저 저장해 use-after-free 막기 (메모리 안전성)
- `lv2-core/rollback-on-partial-failure-in-list-transform.md`: 리스트 변환 도중 할당 실패 시 부분 결과 롤백하기 (자원 관리)

## 경계 있는 문자열

- `lv1/capacity-bounded-copy-reserves-terminator-slot.md`: 버퍼 복사 시 널 종료 자리를 항상 남겨두기 (C 기초)

## 공통 함정

- `lv1/unsigned-char-cast-for-byte-comparisons.md`: signed char 비교/뺄셈에서 부호 확장을 피하기 위한 unsigned char 캐스팅 (C 기초)
