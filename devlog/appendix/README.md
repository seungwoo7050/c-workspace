# Appendix

이 프로젝트를 진행하며 마주친 개념 중 devlog 본문에 담기엔 산발적이었던 것들을 독립된 짧은 문서로 정리한다. 각 문서는 그 자체로 완결되게 읽을 수 있다.

레벨: **Lv1** 신입~주니어 / **Lv2** 미들 / **Lv3** 미들 후반~시니어 직전

각 문서는 레벨별로 `lv1/`, `lv2/`, `lv3/` 디렉터리에 위치한다. 이 중 신입/주니어 채용 실무에서 자주 다뤄지는 항목은 `lv2/` 대신 `lv2-core/`에 별도로 모아뒀다(난이도 자체는 Lv2로 동일, 우선순위만 구분).

**현재 상태**: 락 순서/교착 회피, 조건변수 기반 동기화, TOCTOU 경쟁 상태 방지, 폴링 트레이드오프처럼 스택과 무관하게 어디서든 마주치는 동시성 원칙 5개만 작성했다. 

`appendix/`는 `lv1/`과 `lv2-core/`만 작성했고 `lv2/`(비core)와 `lv3/`는 정의조차 하지 않았다.

## 읽는 순서

1. **동시성 기본 API** `pthread-condition-variable-basics`
2. **교착 회피와 공정한 동시 시작** `dining-philosophers-consistent-lock-ordering` → `condvar-barrier-pattern-for-fair-concurrent-start`
3. **경쟁 상태와 트레이드오프** `check-then-recheck-under-lock-race-window` → `polling-interval-latency-vs-cpu-tradeoff`

## 항목

- `lv1/pthread-condition-variable-basics.md`: pthread 조건변수 기본 API (wait/broadcast, spurious wakeup 방어) (동시성)
- `lv2-core/dining-philosophers-consistent-lock-ordering.md`: 식사하는 철학자 문제의 일관된 락 순서와 교착 회피 (동시성)
- `lv2-core/condvar-barrier-pattern-for-fair-concurrent-start.md`: 조건변수 기반 배리어 패턴으로 공정한 동시 시작 만들기 (동시성)
- `lv2-core/check-then-recheck-under-lock-race-window.md`: 락 밖에서 후보를 찾고 락 안에서 재확인하는 TOCTOU 방지 패턴 (동시성)
- `lv2-core/polling-interval-latency-vs-cpu-tradeoff.md`: 폴링 간격과 지연/CPU 트레이드오프 (동시성/성능)
