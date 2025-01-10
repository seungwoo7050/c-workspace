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
