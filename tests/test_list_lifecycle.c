#include "test.h"

#include "libft.h"

#include <stdlib.h>

static int	g_delete_count;
static int	g_delete_order[4];
static int	g_iterate_count;

static int	*new_integer(int value)
{
	int	*number;

	number = malloc(sizeof(int));
	if (number != NULL)
		*number = value;
	return (number);
}

static void	delete_integer(void *content)
{
	int	*number;

	number = content;
	if (number != NULL && g_delete_count < 4)
		g_delete_order[g_delete_count] = *number;
	g_delete_count++;
	free(number);
}

static void	increment_integer(void *content)
{
	int	*number;

	number = content;
	CHECK(number != NULL);
	if (number != NULL)
		*number += g_iterate_count + 1;
	g_iterate_count++;
}

static t_list	*new_integer_node(int value)
{
	int	*content;
	t_list	*node;

	content = new_integer(value);
	if (content == NULL)
		return (NULL);
	node = ft_lstnew(content);
	if (node == NULL)
		free(content);
	return (node);
}

void	test_list_lifecycle(void)
{
	t_list	*list;
	t_list	*first;
	t_list	*second;
	t_list	*third;
	int	stack_content;

	first = new_integer_node(10);
	second = new_integer_node(20);
	third = new_integer_node(30);
	CHECK(first != NULL && second != NULL && third != NULL);
	if (first == NULL || second == NULL || third == NULL)
	{
		if (first != NULL)
			ft_lstdelone(first, delete_integer);
		if (second != NULL)
			ft_lstdelone(second, delete_integer);
		if (third != NULL)
			ft_lstdelone(third, delete_integer);
		return ;
	}
	list = first;
	first->next = second;
	second->next = third;
	g_iterate_count = 0;
	ft_lstiter(list, increment_integer);
	CHECK(g_iterate_count == 3);
	CHECK(*(int *)first->content == 11);
	CHECK(*(int *)second->content == 22);
	CHECK(*(int *)third->content == 33);
	g_delete_count = 0;
	ft_lstclear(&list, delete_integer);
	CHECK(list == NULL);
	CHECK(g_delete_count == 3);
	CHECK(g_delete_order[0] == 11);
	CHECK(g_delete_order[1] == 22);
	CHECK(g_delete_order[2] == 33);
	first = new_integer_node(44);
	CHECK(first != NULL);
	if (first != NULL)
	{
		g_delete_count = 0;
		ft_lstdelone(first, delete_integer);
		CHECK(g_delete_count == 1 && g_delete_order[0] == 44);
	}
	stack_content = 55;
	first = ft_lstnew(&stack_content);
	CHECK(first != NULL);
	if (first != NULL)
	{
		ft_lstdelone(first, NULL);
		CHECK(first->content == &stack_content && first->next == NULL);
		free(first);
	}
	first = ft_lstnew(&stack_content);
	second = ft_lstnew(&stack_content);
	CHECK(first != NULL && second != NULL);
	if (first != NULL && second != NULL)
	{
		first->next = second;
		list = first;
		ft_lstclear(&list, NULL);
		CHECK(list == first && first->next == second);
		free(second);
		free(first);
	}
	else
	{
		free(first);
		free(second);
	}
	ft_lstclear(NULL, delete_integer);
	ft_lstiter(NULL, increment_integer);
}
