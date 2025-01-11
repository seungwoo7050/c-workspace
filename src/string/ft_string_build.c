#include "libft.h"

#include <stdlib.h>

char	*ft_substr(const char *text, unsigned int start, size_t length)
{
	char	*substring;
	size_t	text_length;

	if (text == NULL)
		return (NULL);
	text_length = ft_strlen(text);
	if ((size_t)start >= text_length)
		return (ft_strdup(""));
	if (length > text_length - (size_t)start)
		length = text_length - (size_t)start;
	substring = malloc(length + 1);
	if (substring == NULL)
		return (NULL);
	ft_memcpy(substring, text + start, length);
	substring[length] = '\0';
	return (substring);
}
