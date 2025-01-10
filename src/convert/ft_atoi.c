#include "libft.h"

#include <limits.h>

static int	is_space(char character)
{
	return (character == ' ' || character == '\t' || character == '\n'
		|| character == '\v' || character == '\f' || character == '\r');
}

int	ft_atoi(const char *text)
{
	unsigned int	value;
	unsigned int	limit;
	int			sign;

	while (is_space(*text))
		text++;
	sign = 1;
	if (*text == '+' || *text == '-')
	{
		if (*text == '-')
			sign = -1;
		text++;
	}
	limit = INT_MAX;
	if (sign < 0)
		limit = (unsigned int)INT_MAX + 1U;
	value = 0;
	while (*text >= '0' && *text <= '9')
	{
		if (value > (limit - (unsigned int)(*text - '0')) / 10U)
			value = limit;
		else
			value = value * 10U + (unsigned int)(*text - '0');
		text++;
	}
	if (sign < 0 && value == (unsigned int)INT_MAX + 1U)
		return (INT_MIN);
	return ((int)value * sign);
}
