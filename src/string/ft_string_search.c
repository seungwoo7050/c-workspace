#include "libft.h"

char	*ft_strchr(const char *text, int character)
{
	unsigned char	target;
	size_t		index;

	target = (unsigned char)character;
	index = 0;
	while (1)
	{
		if ((unsigned char)text[index] == target)
			return ((char *)(text + index));
		if (text[index] == '\0')
			return (NULL);
		index++;
	}
}

char	*ft_strrchr(const char *text, int character)
{
	unsigned char	target;
	char		*match;

	target = (unsigned char)character;
	match = NULL;
	while (1)
	{
		if ((unsigned char)*text == target)
			match = (char *)text;
		if (*text == '\0')
			return (match);
		text++;
	}
}

int	ft_strncmp(const char *left, const char *right, size_t length)
{
	size_t	index;

	index = 0;
	while (index < length)
	{
		if ((unsigned char)left[index] != (unsigned char)right[index])
			return ((int)(unsigned char)left[index]
				- (int)(unsigned char)right[index]);
		if (left[index] == '\0')
			return (0);
		index++;
	}
	return (0);
}

char	*ft_strnstr(const char *haystack, const char *needle, size_t length)
{
	size_t	haystack_index;
	size_t	needle_index;

	if (*needle == '\0')
		return ((char *)haystack);
	haystack_index = 0;
	while (haystack_index < length && haystack[haystack_index] != '\0')
	{
		needle_index = 0;
		while (needle[needle_index] != '\0'
			&& needle_index < length - haystack_index
			&& haystack[haystack_index + needle_index]
				== needle[needle_index])
			needle_index++;
		if (needle[needle_index] == '\0')
			return ((char *)(haystack + haystack_index));
		haystack_index++;
	}
	return (NULL);
}
