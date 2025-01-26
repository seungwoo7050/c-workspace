#include "ft_printf.h"

#include <limits.h>
#include <stdarg.h>
#include <unistd.h>

static int	ft_write_count(const char *buffer, int length, int *count)
{
	int	written;

	if (length <= 0)
		return (0);
	if (*count > INT_MAX - length)
		return (-1);
	written = (int)write(1, buffer, (size_t)length);
	if (written < 0 || written != length)
		return (-1);
	*count += written;
	return (0);
}

int	ft_printf(const char *format, ...)
{
	va_list	args;
	int		count;

	(void)args;
	if (format == 0)
		return (-1);
	va_start(args, format);
	count = 0;
	while (*format)
	{
		if (*format == '%' && *(format + 1) == '%')
		{
			if (ft_write_count("%", 1, &count) < 0)
				count = -1;
			format += 2;
		}
		else
		{
			if (ft_write_count(format, 1, &count) < 0)
				count = -1;
			format++;
		}
		if (count < 0)
			break ;
	}
	va_end(args);
	return (count);
}
