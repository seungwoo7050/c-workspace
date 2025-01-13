#include "libft.h"

#include <stdlib.h>

void	ft_lstdelone(t_list *node, void (*del)(void *))
{
	if (node == NULL || del == NULL)
		return ;
	del(node->content);
	free(node);
}

void	ft_lstclear(t_list **list, void (*del)(void *))
{
	t_list	*next;

	if (list == NULL || del == NULL)
		return ;
	while (*list != NULL)
	{
		next = (*list)->next;
		ft_lstdelone(*list, del);
		*list = next;
	}
}

void	ft_lstiter(t_list *list, void (*function)(void *))
{
	if (function == NULL)
		return ;
	while (list != NULL)
	{
		function(list->content);
		list = list->next;
	}
}
