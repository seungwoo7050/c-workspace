#include "libft.h"

t_list	*ft_lstmap(t_list *list, void *(*function)(void *),
		void (*del)(void *))
{
	t_list	*mapped;
	t_list	*tail;
	t_list	*node;
	void	*mapped_content;

	if (function == NULL || del == NULL)
		return (NULL);
	mapped = NULL;
	tail = NULL;
	while (list != NULL)
	{
		mapped_content = function(list->content);
		node = ft_lstnew(mapped_content);
		if (node == NULL)
		{
			del(mapped_content);
			ft_lstclear(&mapped, del);
			return (NULL);
		}
		if (mapped == NULL)
			mapped = node;
		else
			tail->next = node;
		tail = node;
		list = list->next;
	}
	return (mapped);
}
