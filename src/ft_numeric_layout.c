#include "ft_printf_internal.h"

static int	ft_prefix_length(const char *prefix)
{
	int	length;

	length = 0;
	while (prefix[length])
		length++;
	return (length);
}

int	ft_printf_write_numeric_layout(t_printf *ctx, t_format *fmt,
		const char *prefix, const char *digits, int digit_len, int is_zero)
{
	int		prefix_len;
	int		zero_len;
	int		padding;
	char	pad_char;

	if (fmt->has_precision && fmt->precision == 0 && is_zero)
		digit_len = 0;
	prefix_len = ft_prefix_length(prefix);
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
