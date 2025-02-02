# Appendix

이 프로젝트를 진행하며 마주친 개념 중 devlog 본문에 담기엔 산발적이었던 것들을 독립된 짧은 문서로 정리한다. 각 문서는 그 자체로 완결되게 읽을 수 있다.

레벨: **Lv1** 신입~주니어 / **Lv2** 미들 / **Lv3** 미들 후반~시니어 직전

각 문서는 레벨별로 `lv1/`, `lv2/`, `lv3/` 디렉터리에 위치한다. 이 중 신입/주니어 채용 실무에서 자주 다뤄지는 항목은 `lv2/` 대신 `lv2-core/`에 별도로 모아뒀다(난이도 자체는 Lv2로 동일, 우선순위만 구분).

**현재 상태**: 이 프로젝트는 하나의 명확한 주제를 갖기 때문에 devlog 1개 / appendix 2개로, 채점에서 가장 자주 틀리는 두 함정(`INT_MIN` 처리, `write()` 계약)만 골랐다. 

`appendix/`는 `lv1/`과 `lv2-core/`만 작성했고 `lv2/`(비core)와 `lv3/`는 정의조차 하지 않았다.

## 항목

- `lv1/safely-negating-int-min.md`: INT_MIN을 안전하게 절댓값으로 바꾸는 방법 (C 기초)
- `lv2-core/write-syscall-partial-write-and-eintr-contract.md`: write() 시스템 콜의 부분 쓰기와 EINTR 계약 (시스템)
