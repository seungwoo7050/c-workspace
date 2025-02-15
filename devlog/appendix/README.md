# Appendix

이 프로젝트를 진행하며 마주친 개념 중 devlog 본문에 담기엔 산발적이었던 것들을 독립된 짧은 문서로 정리한다. 각 문서는 그 자체로 완결되게 읽을 수 있다.

레벨: **Lv1** 신입~주니어 / **Lv2** 미들 / **Lv3** 미들 후반~시니어 직전

각 문서는 레벨별로 `lv1/`, `lv2/`, `lv3/` 디렉터리에 위치한다. 이 중 신입/주니어 채용 실무에서 자주 다뤄지는 항목은 `lv2/` 대신 `lv2-core/`에 별도로 모아뒀다(난이도 자체는 Lv2로 동일, 우선순위만 구분).

**현재 상태**: 이 프로젝트는 하나의 명확한 주제(시그널 기반 IPC)라 devlog 2개 / appendix 2개로, "시그널로 어떻게 데이터를 표현하는가"와 "시그널 핸들러 안에서 무엇이 안전한가"라는 두 핵심 질문만 골랐다.

`appendix/`는 `lv1/`과 `lv2-core/`만 작성했고 `lv2/`(비core)와 `lv3/`는 정의조차 하지 않았다.

## 항목

- `lv1/async-signal-safety-limits-what-a-handler-may-call.md`: 시그널 핸들러 안에서 부를 수 있는 함수가 제한되는 이유 (시스템)
- `lv2-core/self-pipe-trick-for-safe-signal-driven-event-loops.md`: self-pipe 트릭으로 시그널을 이벤트 루프 안전하게 흘려보내기 (시스템)
