#include "test.h"

#include "libft.h"

#include <stdlib.h>

void	test_list_basic(void)
{
	int		first_content;
	int		second_content;
	int		third_content;
	t_list		*list;
	t_list		*first;
	t_list		*second;
	t_list		*third;

	first_content = 11;
	second_content = 22;
	third_content = 33;
	list = NULL;
	CHECK(ft_lstsize(list) == 0);
	CHECK(ft_lstlast(list) == NULL);
	first = ft_lstnew(&first_content);
	second = ft_lstnew(&second_content);
	third = ft_lstnew(&third_content);
	CHECK(first != NULL && second != NULL && third != NULL);
	if (first == NULL || second == NULL || third == NULL)
	{
		free(first);
		free(second);
		free(third);
		return ;
	}
	CHECK(first->content == &first_content && first->next == NULL);
	CHECK(second->content == &second_content && second->next == NULL);
	ft_lstadd_front(&list, second);
	ft_lstadd_front(&list, first);
	ft_lstadd_back(&list, third);
	CHECK(list == first);
	CHECK(list->next == second);
	CHECK(second->next == third);
	CHECK(third->next == NULL);
	CHECK(ft_lstsize(list) == 3);
	CHECK(ft_lstlast(list) == third);
	ft_lstadd_front(NULL, NULL);
	ft_lstadd_front(&list, NULL);
	ft_lstadd_back(NULL, NULL);
	ft_lstadd_back(&list, NULL);
	CHECK(ft_lstsize(list) == 3);
	free(first);
	free(second);
	free(third);
	first = ft_lstnew(NULL);
	CHECK(first != NULL);
	if (first != NULL)
	{
		CHECK(first->content == NULL && first->next == NULL);
		free(first);
	}
}
