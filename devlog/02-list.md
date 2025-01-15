# Dev Log 02. Linked list: ownership and rollback

`t_list`(연결 리스트) 관련 함수들(`ft_lstnew`/`ft_lstadd_front`/`ft_lstadd_back`/`ft_lstsize`/`ft_lstlast`)은 대부분 "포인터를 하나씩 따라가며 앞뒤를 잇는다"는 정석적인 조립일 뿐이다. 이 문서는 그 조립보다, 리스트를 "안전하게 해제"하고 "실패해도 일관된 상태로 되돌리는" 것에 집중한다.

## `ft_lstclear`: 해제하기 전에 다음 노드를 먼저 기억해둔다

```c
// src/list/ft_list_lifecycle.c
// t_list **list: "t_list를 가리키는 포인터"를 다시 카리키는 포인터(포인터의 포인터)
// 이 함수가 리스트의 시작 포인터 자체를 NULL로 바꿔야 하므로, 그 포인터가 저장된 주소를 받는다
// void (*del)(void *): "함수를 가리키는 포인터" 매개변수
// del(x)처럼 나중에 호출할 수 있고, 어떤 해제 함수를 쓸지 호출하는 쪽이 직접 정해서 넘겨줄 수 있다
void ft_lstclear(t_list **list, void (*del)(void *)) {
    t_list *next;

    // *list: list가 가리키는 실제 리스트 포인터
    while (*list != NULL) {
        // (*list)->next: *list로 얻은 구조체 포인터에서 -> 로 next 멤버(다음 노드 포인터)에 접근
        next = (*list)->next;     // 먼저 다음 노드를 저장
        ft_lstdelone(*list, del); // 그다음에 현재 노드를 해제
        *list = next;             // 저장해둔 값으로 이동
    }
}
```

순서를 뒤집어서 `ft_lstdelone(*list, del)`을 먼저 부르고 나중에 `(*list)->next`를 읽으면, 이미 `free()`된 메모리를 다시 읽는 use-after-free가 된다. 이는 메모리가 아직 덮어써지지 않았다면 우연히 "동작하는 것처럼 보일" 수도 있어서, 더 위험한 종류의 버그다. **"해제 대상을 참조하는 값은, 해제하기 전에 먼저 꺼내둔다"**는 원칙 하나로 이 문제 전체가 사라진다.

## `ft_lstmap`: 중간에 실패하면 절반짜리 리스트를 남기지 않는다

```c
// src/list/ft_list.c
// void *(*function)(void *): 인자와 반환값이 모두 void*인 함수를 가리키는 함수 포인터
// 어떤 변환 함수든 넘겨받아 쓸 수 있게 해준다
t_list *ft_lstmap(t_list *list, void *(*function)(void *), void (*del)(void *)) {
    ...
    while (list != NULL) {
        // function(...)처럼 함수 포인터 변수도 일반 함수 이름처럼 그대로 호출할 수 있다
        mapped_content = function(list->content);
        node = ft_lstnew(mapped_content);
        // malloc 계열 함수는 실패하면 관례적으로 NULL을 반환하므로 이렇게 확인한다
        if (node == NULL) {
            del(mapped_content); // 이번 변환 결과부터 정리
            // &mapped: mapped 변수의 주소를 넘김
            // ft_lstclear가 t_list**를 받으므로 원본 포인터 변수 자체를 NULL로 바꿀 수 있도록 주소째로 전달
            ft_lstclear(&mapped, del); // 지금까지 만든 새 리스트 전체도 정리
            return (NULL);
        }
        ...
    }
    return (mapped);
}
```

`ft_lstmap`은 원본 리스트를 순회하며 각 노드를 변환한 새 리스트를 만든다. 문제는 그 변환 결과로 새 노드(`ft_lstnew`)를 만드는 `malloc`이 중간에 실패할 수 있다는 것. 이때 지금까지 절반쯤 만든 `mapped` 리스트를 그냥 버리면(포인터만 잃고) 메모리 누수가 되고, 호출자가 `NULL` 반환만 보고 "아무것도 안 만들어졌다"고 착각하면 그 절반짜리 리스트는 영원히 회수되지 않는다. 그래서 실패한 그 순간, 지금까지 만든 것 전부(`mapped`)를 `ft_lstclear`로 정리한 뒤에야 `NULL`을 반환한다. 호출자 입장에서 "`NULL`이 반환됐다 = 아무 자원도 남지 않았다"는 계약이 실제로 성립하게 만드는 것이다.

## 정리

이 두 사례는 서로 다른 실패 상황(해제 순서, 부분 실패)이지만 같은 질문에서 나온다. "지금 이 포인터 연산 이후에, 방금 무효화한 값을 다시 쓰는 경로가 남아있는가?" 링크드 리스트를 직접 구현해보는 과제의 진짜 목적은 포인터 문법 연습이 아니라, 개발 과정에서 버그를 피하기위해 스스로 이런 질문을 습관적으로 던지게 만드는 것이다.
