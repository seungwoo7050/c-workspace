# Dev Log 01. Memory functions: the one worth remembering

과제에서 구현한 메모리 함수군(`ft_memset`/`ft_memcpy`/`ft_memmove`/`ft_memchr`/`ft_memcmp`) 대부분은 "1바이트씩 순회"라는 같은 패턴의 반복이다. 이 문서는 그 반복을 전부 훑는 대신, 프로그래밍 진입 단계에서 실제로 걸려 넘어지기 쉬운 지점만 짚는다.

## 1. `feat(memory): 겹치는 메모리의 안전한 이동 구현`

### `ft_memmove`: 왜 `ft_memcpy` 하나로는 안 되는가

```c
// src/memory/ft_memory_move.c
// void*: "타입을 정하지 않은 포인터", 어떤 자료형의 주소든 담을 수 있지만 그대로 값을 못 읽고 다른 포인터 타입으로 캐스팅해야만 사용 가능
// const void *source: 이 함수 안에서 source가 가리키는 내용을 바꾸지 않겠다는 약속
// size_t: 크기/개수를 표현할 때 사용하는 부호 없는 정수 타입
void *ft_memmove(void *destination, const void *source, size_t length) {
    ...
    offset = 1;
    while (offset < length) {
        // source_byte + offeset처럼 포인터에 정수를 더하면, 그 타입 한 칸 크기만큼 주소가 이동
        if (destination_byte == source_byte + offset) {
            while (length > 0) { // 역순(뒤에서부터) 복사
                length--;
                // 포인터 뒤에 [인덱스]를 붙이면 배열처럼 그 위치의 값을 읽거나 쓸 수 있다
                destination_byte[length] = source_byte[length];
            }
            return (destination);
        }
        offset++;
    }
    ft_memcpy(destination_byte, source_byte, length); // 안 겹치면 정방향에 위임
    return (destination);
}
```

`ft_memcpy`는 앞에서부터 한 바이트씩 정방향으로 복사한다. 그런데 `destination`이 `source`보다 뒤쪽에 있으면서 두 영역이 겹치면(`memmove(buf + 2, buf, 10)` 같은 흔한 패턴) 정방향 복사는 아직 읽지도 않은 뒷부분을 앞부분이 먼저 덮어써버리면서 원본 데이터가 복사 도중에 스스로 손상된다. `ft_memmove`는 `destination`이 `source`보다 몇 바이트 뒤에 있는지(`offset`)를 찾아서, 그 겹침이 있으면 뒤에서부터 역순으로 복사해 "읽기 전에 덮어쓰는" 상황 자체를 없앤다. 겹치지 않으면 이미 검증된 `ft_memcpy`에 그대로 위임한다.

이 구현의 겹침 탐지는 `offset`을 1부터 `length`까지 순회하며 매번 포인터를 비교하는 방식이라 $O(n)$이다. 실제 libc 구현들은 보통 `destination > source`라는 포인터 비교 하나로 O(1)에 같은 판단을 내린다. 더 단순하면서도 정석적인 방법을 알 지 못한 나의 실수였다.

## 2. `feat(memory): 범위를 제한한 메모리 검색과 비교 추가`

### `ft_memcmp`: 뺄셈으로 비교 결과를 표현할 때의 함정

```c
// src/memory/ft_memory_scan.c
int ft_memcmp(const void *left, const void *right, size_t length) {
    ...
    if (left_byte[index] != right_byte[index])
        // unsigned 값끼리 그냥 빼면 unsigned 연산이라 음수가 나올 자리에서 큰 양수로 뒤집힐 수 있다
        // (int)로 먼저 캐스팅해서 부호 있는 정수 뺄셈으로 바꿔야 결과 부호가 안전하게 나온다
        return ((int)left_byte[index] - (int)right_byte[index]);
    ...
}
```

`unsigned char` 값을 그대로 빼면(`left_byte[index] - right_byte[index]`) `unsigned` 연산이라 언더플로가 나서(예: `0 - 255`가 큰 양수로 wrap) 부호가 뒤집힐 수 있다. `(int)`로 먼저 캐스팅한 뒤 빼야, 0~255 범위의 값이 부호 있는 정수 범위 안에서 안전하게 뺄셈되어 "왼쪽이 더 크면 양수, 작으면 음수"라는 `memcmp`의 계약이 실제로 지켜진다.

## 정리

"포인터/바이트 값을 다룰 때, signed/unsigned 경계를 넘나드는 연산은 캐스팅 순서와 타이밍이 결과를 바꾼다." 이건 과제 전체(`ft_strchr`의 `unsigned char` 캐스팅 등)에서 반복되었던, 프로그래밍 입문 단계의 가장 흔한 함정 중 하나다.
