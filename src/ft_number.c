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
	int		prefix_len;
	int		zero_len;
	int		padding;
	char	pad_char;

	digit_len = ft_decimal_digits(digits, number);
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
