#include "ft_printf_internal.h"

int	ft_printf(const char *format, ...)
{
	va_list	args;
	va_list	measure_args;
	t_printf	ctx;
	t_format	fmt;

	if (format == 0)
		return (-1);
	va_start(args, format);
	va_copy(measure_args, args);
	if (ft_printf_measure(format, &measure_args) < 0)
	{
		va_end(measure_args);
		va_end(args);
		return (-1);
	}
	va_end(measure_args);
	ft_printf_init(&ctx, 1);
	while (*format)
	{
		if (*format == '%' && *(format + 1) == '%')
		{
			if (ft_printf_putchar(&ctx, '%') < 0)
				break ;
			format += 2;
		}
		else if (*format == '%')
		{
			format = ft_printf_parse(format + 1, &fmt);
			if (format == 0)
			{
				ctx.error = 1;
				break ;
			}
			if (ft_printf_dispatch(&ctx, &fmt, &args) < 0)
				break ;
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
