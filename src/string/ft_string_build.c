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

char	*ft_strjoin(const char *left, const char *right)
{
	char	*joined;
	size_t	left_length;
	size_t	right_length;

	if (left == NULL || right == NULL)
		return (NULL);
	left_length = ft_strlen(left);
	right_length = ft_strlen(right);
	if (right_length == (size_t)-1
		|| left_length > (size_t)-2 - right_length)
		return (NULL);
	joined = malloc(left_length + right_length + 1);
	if (joined == NULL)
		return (NULL);
	ft_memcpy(joined, left, left_length);
	ft_memcpy(joined + left_length, right, right_length + 1);
	return (joined);
}
