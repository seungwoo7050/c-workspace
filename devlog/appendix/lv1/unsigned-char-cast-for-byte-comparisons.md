# signed char 비교/뺄셈에서 부호 확장을 피하기 위한 unsigned char 캐스팅

**Lv1 / C 기초**

**프로젝트에서** `ft_memcmp`(`src/memory/ft_memory_scan.c`)와 `ft_strchr`/`ft_strncmp`(`src/string/`)가 전부 문자를 비교하기 전에 `unsigned char`로 캐스팅한다.

```c
// ft_memcmp
if (left_byte[index] != right_byte[index])
    return ((int)left_byte[index] - (int)right_byte[index]);

// ft_strchr
target = (unsigned char)c;
if ((unsigned char)text[idx] == target) ...
```

**일반적으로** 대부분의 플랫폼에서 `char`는 `signed`다(표준이 강제하지 않고 구현체에 맡긴 부분이라 반대인 플랫폼도 있다). 0x80 이상의 바이트 값(예: 0xFF)을 `signed char`로 다루면 그 값은 음수(-1)로 해석된다. 이 상태로 비교나 뺄셈을 하면 두 가지 문제가 생긴다.

1. `signed char`를 `int`로 승격시키면 부호 확장(sign extension)이 일어나 원래 의도한 "0~255 범위의 바이트 값"이 아니라 음수 `int`가 된다.

2. `memcmp` 계열 함수는 "바이트 값의 크기를 부호 없는 값으로 비교한다"는 계약을 갖고 있어서, 부호 있는 값으로 비교/뺄셈하면 그 계약이 깨진다(0xFF가 0x01보다 작다고 잘못 판단하는 식).

`(unsigned char)`로 먼저 캐스팅하면 0~255 범위의 값으로 고정되어 이 문제가 사라진다.

**모범사례**:  
이 패턴은 딱히 "더 나은 대안"이 있는 종류의 트레이드오프가 아니라, 표준 라이브러리(`memcmp`, `strcmp`, `ctype.h` 함수군)가 전부 이 캐스팅 규약을 명시적으로 요구하는 영역이다. 실무에서 이 캐스팅을 빠뜨리는 실수는 대개 ASCII 범위(0~127) 안의 데이터만 다룰 땐 절대 드러나지 않다가, 확장 아스키나 UTF-8의 멀티바이트 시퀀스(항상 최상위 비트가 1)처럼 0x80 이상의 바이트가 실제로 섞이는 순간에만 조용히 잘못된 결과를 낸다. "지금 당장은 안 터진다"가 "이 코드가 맞다"를 보장하지 않는다는 걸 보여주는 전형적인 사례다.
