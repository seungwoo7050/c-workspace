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

	if (fmt->spec == 'X')
		digit_len = ft_hex_digits(digits, number, "0123456789ABCDEF");
	else
		digit_len = ft_hex_digits(digits, number, "0123456789abcdef");
	return (ft_printf_write_numeric_layout(ctx, fmt, prefix, digits,
			digit_len, number == 0));
}

int	ft_printf_print_hex(t_printf *ctx, t_format *fmt, unsigned int number)
{
	const char	*prefix;

	prefix = "";
	if ((fmt->flags & FT_FLAG_HASH) && number != 0)
	{
		if (fmt->spec == 'X')
			prefix = "0X";
		else
			prefix = "0x";
	}
	return (ft_write_hex(ctx, fmt, prefix, (unsigned long)number));
}

int	ft_printf_print_pointer(t_printf *ctx, t_format *fmt, void *pointer)
{
	return (ft_write_hex(ctx, fmt, "0x",
			(unsigned long)(uintptr_t)pointer));
}
