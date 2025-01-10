#include "libft.h"

#include <stdlib.h>

void	*ft_calloc(size_t count, size_t size)
{
	size_t	allocation_size;
	void	*allocation;

	if (count != 0 && size > (size_t)-1 / count)
		return (NULL);
	allocation_size = count * size;
	if (allocation_size == 0)
		allocation_size = 1;
	allocation = malloc(allocation_size);
	if (allocation == NULL)
		return (NULL);
	ft_bzero(allocation, allocation_size);
	return (allocation);
}

char	*ft_strdup(const char *text)
{
	char	*duplicate;
	size_t	length;

	if (text == NULL)
		return (NULL);
	length = ft_strlen(text);
	if (length == (size_t)-1)
		return (NULL);
	duplicate = malloc(length + 1);
	if (duplicate == NULL)
		return (NULL);
	ft_memcpy(duplicate, text, length + 1);
	return (duplicate);
}
