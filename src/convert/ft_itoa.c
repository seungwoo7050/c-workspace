#include "libft.h"

#include <stdlib.h>

static size_t	digit_count(unsigned int magnitude)
{
	size_t	count;

	count = 1;
	while (magnitude >= 10U)
	{
		magnitude /= 10U;
		count++;
	}
	return (count);
}

char	*ft_itoa(int number)
{
	char		*text;
	unsigned int	magnitude;
	size_t		length;

	if (number < 0)
		magnitude = (unsigned int)(-(number + 1)) + 1U;
	else
		magnitude = (unsigned int)number;
	length = digit_count(magnitude) + (number < 0);
	text = malloc(length + 1);
	if (text == NULL)
		return (NULL);
	text[length] = '\0';
	while (length > (size_t)(number < 0))
	{
		length--;
		text[length] = (char)('0' + magnitude % 10U);
		magnitude /= 10U;
	}
	if (number < 0)
		text[0] = '-';
	return (text);
}
