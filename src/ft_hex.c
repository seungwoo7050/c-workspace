#include "ft_printf_internal.h"

#include <stdint.h>

static int	ft_print_base(t_printf *ctx, unsigned long number, const char *digits)
{
	char	buffer[2 + sizeof(unsigned long) * 2];
	int		index;

	index = 0;
	if (number == 0)
		buffer[index++] = '0';
	while (number > 0)
	{
		buffer[index++] = digits[number % 16];
		number /= 16;
	}
	while (index > 0)
	{
		index--;
		if (ft_printf_putchar(ctx, buffer[index]) < 0)
			return (-1);
	}
	return (0);
}

int	ft_printf_print_hex(t_printf *ctx, t_format *fmt, unsigned int number)
{
	if (fmt->spec == 'X')
		return (ft_print_base(ctx, (unsigned long)number, "0123456789ABCDEF"));
	return (ft_print_base(ctx, (unsigned long)number, "0123456789abcdef"));
}

int	ft_printf_print_pointer(t_printf *ctx, t_format *fmt, void *pointer)
{
	(void)fmt;
	if (ft_printf_write(ctx, "0x", 2) < 0)
		return (-1);
	return (ft_print_base(ctx, (unsigned long)(uintptr_t)pointer,
			"0123456789abcdef"));
}
