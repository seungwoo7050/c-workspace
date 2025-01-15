# 겹치는 메모리 영역을 안전하게 옮기기 위한 방향 탐지

**Lv2-core / 메모리**

**프로젝트에서** `src/memory/ft_memory_move.c`의 `ft_memmove()`가 목적지가 소스보다 뒤쪽에서 겹치는지 `offset`으로 탐지해, 겹치면 뒤에서부터(역순), 안 겹치면 `ft_memcpy`(정방향)로 위임한다.

```c
offset = 1;
while (offset < length) {
    if (destination_byte == source_byte + offset) {
        while (length > 0) { length--; destination_byte[length] = source_byte[length]; }
        return (destination);
    }
    offset++;
}
ft_memcpy(destination_byte, source_byte, length);
```

**일반적으로** 두 메모리 영역이 겹칠 때 정방향 복사를 그대로 쓰면, 아직 읽지 않은 뒤쪽 원본 바이트를 앞쪽 복사가 먼저 덮어써서 데이터가 손상된다. 안전하려면 "겹침의 방향"에 따라 복사 방향을 반대로 정해야 한다. 목적지가 소스보다 앞이면 정방향, 뒤면 역방향으로 복사해야 아직 안 읽은 값을 덮어쓰는 상황 자체가 생기지 않는다. `memmove`가 표준 라이브러리에 `memcpy`와 별도로 존재하는 이유가 이것이다. `memcpy`는 겹치지 않는다는 전제 위에서만 동작이 정의돼 있고, 겹치는 입력을 넣으면 undefined behavior다.

**모범사례**:  
이 구현은 겹침 여부와 방향을 `offset`을 1부터 `length`까지 순회하며 포인터를 비교해 판별한다($O(n)$). 실제 libc 구현들은 대개 `destination > source`(포인터 자체의 대소 비교, $O(1)$)만으로 같은 판단을 내린다. 두 포인터가 같은 배열/버퍼를 가리킨다는 전제가 있으면, 포인터 값의 대소가 곧 "어느 쪽이 메모리 주소상 더 뒤인가"를 그대로 알려주기 때문이다. 이 프로젝트의 방식이 틀린 건 아니지만(같은 결론에 도달한다), 더 단순하고 빠른 동치 판단법이 존재한다는 걸 아는 것 자체가 "동작하는 코드"와 "잘 아는 코드"의 차이를 보여주는 지점이다.
