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
	int		zero_len;
	int		padding;
	char	pad_char;

	if (fmt->spec == 'X')
		digit_len = ft_hex_digits(digits, number, "0123456789ABCDEF");
	else
		digit_len = ft_hex_digits(digits, number, "0123456789abcdef");
	if (fmt->has_precision && fmt->precision == 0 && number == 0)
		digit_len = 0;
	prefix_len = 0;
	while (prefix[prefix_len])
		prefix_len++;
	zero_len = 0;
	if (fmt->has_precision && fmt->precision > digit_len)
		zero_len = fmt->precision - digit_len;
	padding = fmt->width - prefix_len - zero_len - digit_len;
	pad_char = ' ';
	if ((fmt->flags & FT_FLAG_ZERO) && !(fmt->flags & FT_FLAG_LEFT)
		&& !fmt->has_precision)
		pad_char = '0';
	if (!(fmt->flags & FT_FLAG_LEFT)
		&& pad_char == ' '
		&& ft_printf_putnchar(ctx, ' ', padding) < 0)
		return (-1);
	if (ft_printf_write(ctx, prefix, (size_t)prefix_len) < 0)
		return (-1);
	if (!(fmt->flags & FT_FLAG_LEFT)
		&& pad_char == '0'
		&& ft_printf_putnchar(ctx, '0', padding) < 0)
		return (-1);
	if (ft_printf_putnchar(ctx, '0', zero_len) < 0)
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
