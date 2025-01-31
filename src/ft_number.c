#include "ft_printf_internal.h"

static int	ft_decimal_digits(char *buffer, unsigned long number)
{
	char	reversed[20];
	int		length;
	int		index;

	length = 0;
	if (number == 0)
		reversed[length++] = '0';
	while (number > 0)
	{
		reversed[length++] = (char)('0' + number % 10);
		number /= 10;
	}
	index = 0;
	while (index < length)
	{
		buffer[index] = reversed[length - 1 - index];
		index++;
	}
	return (length);
}

static int	ft_write_decimal(t_printf *ctx, t_format *fmt,
		const char *prefix, unsigned long number)
{
	char	digits[20];
	int		digit_len;

	digit_len = ft_decimal_digits(digits, number);
	return (ft_printf_write_numeric_layout(ctx, fmt, prefix, digits,
			digit_len, number == 0));
}

int	ft_printf_print_signed(t_printf *ctx, t_format *fmt, int number)
{
	long	value;
	const char	*prefix;

	value = (long)number;
	if (value < 0)
		return (ft_write_decimal(ctx, fmt, "-", (unsigned long)(-value)));
	prefix = "";
	if (fmt->flags & FT_FLAG_PLUS)
		prefix = "+";
	else if (fmt->flags & FT_FLAG_SPACE)
		prefix = " ";
	return (ft_write_decimal(ctx, fmt, prefix, (unsigned long)value));
}

int	ft_printf_print_unsigned(t_printf *ctx, t_format *fmt, unsigned int number)
{
	return (ft_write_decimal(ctx, fmt, "", (unsigned long)number));
}
