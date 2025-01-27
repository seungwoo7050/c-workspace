#include "ft_printf_internal.h"

#include <stdarg.h>

int	ft_printf(const char *format, ...)
{
	va_list	args;
	t_printf	ctx;

	(void)args;
	if (format == 0)
		return (-1);
	va_start(args, format);
	ft_printf_init(&ctx, 1);
	while (*format)
	{
		if (*format == '%' && *(format + 1) == '%')
		{
			if (ft_printf_putchar(&ctx, '%') < 0)
				break ;
			format += 2;
		}
		else
		{
			if (ft_printf_putchar(&ctx, *format) < 0)
				break ;
			format++;
		}
	}
	va_end(args);
	if (ctx.error)
		return (-1);
	return (ctx.count);
}
