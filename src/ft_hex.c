#include "ft_printf_internal.h"

#include <stdint.h>

static int	ft_hex_digits(char *buffer, unsigned long number, const char *base)
{
	char	reversed[2 + sizeof(unsigned long) * 2];
	int		length;
	int		index;

	length = 0;
	if (number == 0)
		reversed[length++] = '0';
	while (number > 0)
	{
		reversed[length++] = base[number % 16];
		number /= 16;
	}
	index = 0;
	while (index < length)
	{
		buffer[index] = reversed[length - 1 - index];
		index++;
	}
	return (length);
}

static int	ft_write_hex(t_printf *ctx, t_format *fmt, const char *prefix,
		unsigned long number)
{
	char	digits[2 + sizeof(unsigned long) * 2];
	int		digit_len;
	int		prefix_len;
	int		padding;

	if (fmt->spec == 'X')
		digit_len = ft_hex_digits(digits, number, "0123456789ABCDEF");
	else
		digit_len = ft_hex_digits(digits, number, "0123456789abcdef");
	prefix_len = 0;
	while (prefix[prefix_len])
		prefix_len++;
	padding = fmt->width - prefix_len - digit_len;
	if (!(fmt->flags & FT_FLAG_LEFT)
		&& ft_printf_putnchar(ctx, ' ', padding) < 0)
		return (-1);
	if (ft_printf_write(ctx, prefix, (size_t)prefix_len) < 0)
		return (-1);
	if (ft_printf_write(ctx, digits, (size_t)digit_len) < 0)
		return (-1);
	if ((fmt->flags & FT_FLAG_LEFT)
		&& ft_printf_putnchar(ctx, ' ', padding) < 0)
		return (-1);
	return (0);
}

int	ft_printf_print_hex(t_printf *ctx, t_format *fmt, unsigned int number)
{
	return (ft_write_hex(ctx, fmt, "", (unsigned long)number));
}

int	ft_printf_print_pointer(t_printf *ctx, t_format *fmt, void *pointer)
{
	return (ft_write_hex(ctx, fmt, "0x",
			(unsigned long)(uintptr_t)pointer));
}
