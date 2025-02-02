# write() 시스템 콜의 부분 쓰기와 EINTR 계약

**Lv2-core / 시스템**

**프로젝트에서** `src/ft_output.c`의 `ft_printf_write()`가 `write()`를 한 번만 부르고 끝내지 않고, 요청한 길이를 다 쓸 때까지 반복하며 세 가지 예외 상황을 방어한다.

```c
while (length > 0) {
    request = length;
    if (request > (size_t)SSIZE_MAX)
        request = (size_t)SSIZE_MAX;
    written = write(ctx->fd, buffer, request);
    if (written < 0 && errno == EINTR)
        continue ;
    if (written <= 0) { ctx->error = 1; return (-1); }
    buffer += written;
    length -= (size_t)written;
}
```

**일반적으로** `write(2)`의 실제 계약은 "성공하면 0 이상, 요청한 크기 이하의 바이트 수만큼 썼다"는 것뿐이다. 요청한 만큼 전부 썼다는 보장이 없다(파이프 버퍼가 꽉 찼거나, 디스크가 일시적으로 느리거나 하는 이유로 일부만 쓰고 반환할 수 있다). 그래서 반환값을 확인하지 않고 "한 번 불렀으니 다 썼겠지"라고 가정하면 데이터가 조용히 잘린다. 또한 그 호출이 시그널에 의해 중단되면 `-1`을 반환하고 `errno`를 `EINTR`로 설정하는데, 이건 "실패"가 아니라 "다시 시도해야 한다"는 신호다. 이걸 다른 진짜 에러(디스크 꽉 참 등)와 같은 방식으로 처리하면 정상적으로 처리 가능했던 상황까지 실패로 잘못 보고하게 된다. `SSIZE_MAX`를 넘는 크기 요청에 대한 동작은 POSIX가 아예 규정하지 않으므로, 그 이상은 요청 자체를 잘라서 여러 번의 호출로 나눈다.

**모범사례**:  
이 "부분 쓰기까지 반복 + EINTR 재시도 + 크기 클램프"는 표준 라이브러리의 `write()` 위에 안전하게 쌓아야 하는 필수 보일러플레이트다. 실무 코드에서는 이런 반복문을 함수마다 따로 짜는 대신 "쓸 때까지 반복하는 `write_full`/`writeAll`" 같은 공용 헬퍼 하나로 뽑아 재사용하는 경우가 많다(이 프로젝트의 `ft_printf_write` 자체가 그 역할을 한다). 매번 새로 짜면 그중 하나(특히 부분 쓰기 처리)를 빠뜨리기 쉽기 때문이다.
