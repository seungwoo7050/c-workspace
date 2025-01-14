#include "test.h"

#include "libft.h"

#include <stdlib.h>

static int	g_map_delete_count;

static void	*double_integer(void *content)
{
	int	*mapped;

	mapped = malloc(sizeof(int));
	if (mapped != NULL)
		*mapped = *(int *)content * 2;
	return (mapped);
}

static void	*map_null_content(void *content)
{
	(void)content;
	return (NULL);
}

static void	delete_mapped_integer(void *content)
{
	g_map_delete_count++;
	free(content);
}

static void	keep_source_content(void *content)
{
	(void)content;
}

void	test_list_map(void)
{
	int		a;
	int		b;
	int		c;
	t_list		*source;
	t_list		*mapped;
	t_list		*first;
	t_list		*second;
	t_list		*third;

	a = 3;
	b = 5;
	c = 8;
	first = ft_lstnew(&a);
	second = ft_lstnew(&b);
	third = ft_lstnew(&c);
	CHECK(first != NULL && second != NULL && third != NULL);
	if (first == NULL || second == NULL || third == NULL)
	{
		free(first);
		free(second);
		free(third);
		return ;
	}
	source = first;
	first->next = second;
	second->next = third;
	mapped = ft_lstmap(source, double_integer, delete_mapped_integer);
	CHECK(mapped != NULL);
	if (mapped != NULL)
	{
		CHECK(ft_lstsize(mapped) == 3);
		CHECK(mapped != source && mapped->next != second);
		CHECK(mapped->content != source->content);
		CHECK(*(int *)mapped->content == 6);
		CHECK(*(int *)mapped->next->content == 10);
		CHECK(*(int *)mapped->next->next->content == 16);
		CHECK(a == 3 && b == 5 && c == 8);
		g_map_delete_count = 0;
		ft_lstclear(&mapped, delete_mapped_integer);
		CHECK(mapped == NULL && g_map_delete_count == 3);
	}
	g_map_delete_count = 0;
	mapped = ft_lstmap(source, map_null_content, delete_mapped_integer);
	CHECK(mapped != NULL);
	if (mapped != NULL)
	{
		CHECK(ft_lstsize(mapped) == 3);
		CHECK(mapped->content == NULL);
		CHECK(mapped->next->content == NULL);
		CHECK(mapped->next->next->content == NULL);
		ft_lstclear(&mapped, delete_mapped_integer);
		CHECK(mapped == NULL && g_map_delete_count == 3);
	}
	CHECK(ft_lstmap(source, NULL, delete_mapped_integer) == NULL);
	CHECK(ft_lstmap(source, double_integer, NULL) == NULL);
	CHECK(ft_lstmap(NULL, double_integer, delete_mapped_integer) == NULL);
	ft_lstclear(&source, keep_source_content);
	CHECK(source == NULL);
}
