#include "ft_printf_internal.h"

int	ft_printf_dispatch(t_printf *ctx, t_format *fmt, va_list *args)
{
	if (fmt->spec == 'c')
		return (ft_printf_print_char(ctx, fmt, va_arg(*args, int)));
	if (fmt->spec == 's')
		return (ft_printf_print_string(ctx, fmt, va_arg(*args, char *)));
	if (fmt->spec == 'd' || fmt->spec == 'i')
		return (ft_printf_print_signed(ctx, fmt, va_arg(*args, int)));
	if (fmt->spec == 'u')
		return (ft_printf_print_unsigned(ctx, fmt,
				va_arg(*args, unsigned int)));
	if (fmt->spec == '%')
		return (ft_printf_print_percent(ctx, fmt));
	if (ft_printf_putchar(ctx, '%') < 0)
		return (-1);
	if (fmt->spec)
		return (ft_printf_putchar(ctx, fmt->spec));
	return (0);
}
