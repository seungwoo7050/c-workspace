#include "ft_printf_internal.h"

#include <limits.h>

static int	ft_is_digit(char c)
{
	return (c >= '0' && c <= '9');
}

static int	ft_flag_value(char c)
{
	if (c == '-')
		return (FT_FLAG_LEFT);
	if (c == '0')
		return (FT_FLAG_ZERO);
	if (c == '#')
		return (FT_FLAG_HASH);
	if (c == ' ')
		return (FT_FLAG_SPACE);
	if (c == '+')
		return (FT_FLAG_PLUS);
	return (0);
}

static int	ft_parse_decimal(const char **format, int *value)
{
	int	digit;

	while (ft_is_digit(**format))
	{
		digit = **format - '0';
		if (*value > (INT_MAX - digit) / 10)
			return (-1);
		*value = *value * 10 + digit;
		(*format)++;
	}
	return (0);
}

void	ft_printf_init_format(t_format *fmt)
{
	fmt->flags = 0;
	fmt->width = 0;
	fmt->precision = 0;
	fmt->has_precision = 0;
	fmt->spec = '\0';
}

const char	*ft_printf_parse(const char *format, t_format *fmt)
{
	int	flag;

	ft_printf_init_format(fmt);
	flag = ft_flag_value(*format);
	while (flag)
	{
		fmt->flags |= flag;
		format++;
		flag = ft_flag_value(*format);
	}
	if (ft_parse_decimal(&format, &fmt->width) < 0)
		return (0);
	if (*format == '.')
	{
		fmt->has_precision = 1;
		format++;
		if (ft_parse_decimal(&format, &fmt->precision) < 0)
			return (0);
	}
	fmt->spec = *format;
	if (*format)
		format++;
	return (format);
}
