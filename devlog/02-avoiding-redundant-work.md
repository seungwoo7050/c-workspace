# Dev Log 02. Two ways this reader avoids doing the same work twice

## 커서를 저장해서 매번 처음부터 다시 훑지 않는다

```c
static size_t find_line_end(t_reader *reader) {
    while (reader->scan < reader->end) { // begin이 아니라 scan부터 이어서 검사
        if (reader->bytes[reader->scan] == '\n') {
            reader->scan++;
            return (reader->scan);
        }
        reader->scan++;
    }
    return (0);
}
```

개행 문자를 찾는 탐색이 `begin`(아직 반환 안 한 데이터의 시작)이 아니라 `scan`(지난번까지 이미 검사해서 개행이 없다고 확인된 지점)부터 시작한다. 개행이 아직 안 나온 상태에서 여러 번 `read()`를 반복해 버퍼를 계속 키워야 하는 긴 줄의 경우, `scan`이 없으면 매 `read()` 이후 버퍼 전체를 처음부터 다시 훑어야 해서 총 비교 횟수가 버퍼 길이에 대해 제곱으로 늘어난다($O(n^2)$). `scan`을 유지하면 각 바이트는 정확히 한 번만 검사되어 전체가 $O(n)$으로 줄어든다.

## 임시 버퍼에 복사했다가 다시 옮기는 대신, 목적지에 바로 읽는다

```c
// 이전: 스택 버퍼로 read() → 그 결과를 다시 힙 버퍼로 memcpy(append_bytes)
char buffer[BUFFER_SIZE];
read(fd, buffer, BUFFER_SIZE);
append_bytes(reader, buffer, read_size); // 여기서 한 번 더 복사

// 이후: 힙 버퍼의 빈 공간에 곧바로 read()
reserve_bytes(reader, BUFFER_SIZE);
// read()의 두 번째 인자는 "쓸 위치의 주소"이기만 하면 되므로, 
// 배열의 시작이 아니라 힙 버퍼 중간의 특정 오프셋(reader->bytes + reader->end)을
// 직접 넘겨 그 자리에서 바로 쓰게 할 수 있다
read(fd, reader->bytes + reader->end, BUFFER_SIZE); // 복사 없이 바로 그 자리에 씀
```

`read()`가 스택의 임시 배열에 먼저 쓰고, 그 내용을 다시 누적 버퍼로 복사하던(`append_bytes`) 2단계 경로를, "누적 버퍼의 빈 공간 크기만큼 미리 확보해두고 그 자리에 바로 `read()`한다"는 1단계 경로로 줄였다. 매 반복마다 있던 `memcpy` 한 번이 통째로 사라진다. 기능은 동일하지만 불필요한 중간 복사를 없앤 전형적인 성능 리팩터다.

## 정리

두 사례 모두 "정답은 이미 맞았다"는 공통점이 있다. scan 없이도, 임시 버퍼를 거쳐도 결과적으로 같은 줄을 반환한다. 하지만 "매번 처음부터 다시 하는가"와 "이미 아는 걸 다시 확인하지 않는가"의 차이가 입력 크기가 커질수록 실질적인 비용 차이로 드러난다. 작은 입력에서만 테스트해보면 절대 드러나지 않는 종류의 문제다.
