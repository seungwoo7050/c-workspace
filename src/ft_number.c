#include "ft_printf_internal.h"

static int	ft_print_unsigned_digits(t_printf *ctx, unsigned long number)
{
	char	buffer[20];
	int		index;

	index = 0;
	if (number == 0)
		buffer[index++] = '0';
	while (number > 0)
	{
		buffer[index++] = (char)('0' + number % 10);
		number /= 10;
	}
	while (index > 0)
	{
		index--;
		if (ft_printf_putchar(ctx, buffer[index]) < 0)
			return (-1);
	}
	return (0);
}

int	ft_printf_print_signed(t_printf *ctx, t_format *fmt, int number)
{
	long	value;

	(void)fmt;
	value = (long)number;
	if (value < 0)
	{
		if (ft_printf_putchar(ctx, '-') < 0)
			return (-1);
		value = -value;
	}
	return (ft_print_unsigned_digits(ctx, (unsigned long)value));
}

int	ft_printf_print_unsigned(t_printf *ctx, t_format *fmt, unsigned int number)
{
	(void)fmt;
	return (ft_print_unsigned_digits(ctx, (unsigned long)number));
}
