#include "libft.h"

#include <stdlib.h>

char	*ft_strmapi(const char *text,
		char (*function)(unsigned int, char))
{
	char		*mapped;
	size_t		length;
	size_t		index;

	if (text == NULL || function == NULL)
		return (NULL);
	length = ft_strlen(text);
	mapped = malloc(length + 1);
	if (mapped == NULL)
		return (NULL);
	index = 0;
	while (index < length)
	{
		mapped[index] = function((unsigned int)index, text[index]);
		index++;
	}
	mapped[index] = '\0';
	return (mapped);
}

void	ft_striteri(char *text, void (*function)(unsigned int, char *))
{
	size_t		length;
	size_t		index;

	if (text == NULL || function == NULL)
		return ;
	length = ft_strlen(text);
	index = 0;
	while (index < length)
	{
		function((unsigned int)index, text + index);
		index++;
	}
}
