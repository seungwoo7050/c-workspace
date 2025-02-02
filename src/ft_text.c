#include "ft_printf_internal.h"

static size_t	ft_local_strlen(const char *string, t_format *fmt)
{
	size_t	length;

	length = 0;
	while ((!fmt->has_precision || length < (size_t)fmt->precision)
		&& string[length])
		length++;
	return (length);
}

int	ft_printf_print_char(t_printf *ctx, t_format *fmt, int c)
{
	int	padding;

	padding = fmt->width - 1;
	if (!(fmt->flags & FT_FLAG_LEFT)
		&& ft_printf_putnchar(ctx, ' ', padding) < 0)
		return (-1);
	if (ft_printf_putchar(ctx, (char)c) < 0)
		return (-1);
	if ((fmt->flags & FT_FLAG_LEFT)
		&& ft_printf_putnchar(ctx, ' ', padding) < 0)
		return (-1);
	return (0);
}

int	ft_printf_print_string(t_printf *ctx, t_format *fmt, const char *string)
{
	size_t	length;
	int		padding;

	if (string == 0)
		string = "(null)";
	length = ft_local_strlen(string, fmt);
	padding = fmt->width - (int)length;
	if (!(fmt->flags & FT_FLAG_LEFT)
		&& ft_printf_putnchar(ctx, ' ', padding) < 0)
		return (-1);
	if (ft_printf_write(ctx, string, length) < 0)
		return (-1);
	if ((fmt->flags & FT_FLAG_LEFT)
		&& ft_printf_putnchar(ctx, ' ', padding) < 0)
		return (-1);
	return (0);
}

int	ft_printf_print_percent(t_printf *ctx, t_format *fmt)
{
	int	padding;
	char	pad_char;

	padding = fmt->width - 1;
	pad_char = ' ';
	if ((fmt->flags & FT_FLAG_ZERO) && !(fmt->flags & FT_FLAG_LEFT))
		pad_char = '0';
	if (!(fmt->flags & FT_FLAG_LEFT)
		&& ft_printf_putnchar(ctx, pad_char, padding) < 0)
		return (-1);
	if (ft_printf_putchar(ctx, '%') < 0)
		return (-1);
	if ((fmt->flags & FT_FLAG_LEFT)
		&& ft_printf_putnchar(ctx, ' ', padding) < 0)
		return (-1);
	return (0);
}
