#include "ft_printf_internal.h"

static size_t	ft_local_strlen(const char *string)
{
	size_t	length;

	length = 0;
	while (string[length])
		length++;
	return (length);
}

int	ft_printf_print_char(t_printf *ctx, t_format *fmt, int c)
{
	(void)fmt;
	return (ft_printf_putchar(ctx, (char)c));
}

int	ft_printf_print_string(t_printf *ctx, t_format *fmt, const char *string)
{
	(void)fmt;
	if (string == 0)
		string = "(null)";
	return (ft_printf_write(ctx, string, ft_local_strlen(string)));
}

int	ft_printf_print_percent(t_printf *ctx, t_format *fmt)
{
	(void)fmt;
	return (ft_printf_putchar(ctx, '%'));
}
