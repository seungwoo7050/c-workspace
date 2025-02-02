#include "ft_printf_internal.h"

#include <limits.h>
#include <stdint.h>

static int	ft_is_supported_specifier(char spec)
{
	return (spec == 'c' || spec == 's' || spec == 'd' || spec == 'i'
		|| spec == 'u' || spec == 'x' || spec == 'X' || spec == 'p'
		|| spec == '%');
}

static int	ft_add_length(size_t *total, size_t amount)
{
	if (amount > (size_t)INT_MAX || *total > (size_t)INT_MAX - amount)
		return (-1);
	*total += amount;
	return (0);
}

static int	ft_decimal_length(unsigned long number)
{
	int	length;

	length = 1;
	while (number >= 10)
	{
		number /= 10;
		length++;
	}
	return (length);
}

static int	ft_hex_length(unsigned long number)
{
	int	length;

	length = 1;
	while (number >= 16)
	{
		number /= 16;
		length++;
	}
	return (length);
}

static int	ft_measure_numeric(t_format *fmt, int prefix_length,
		int digit_length, int is_zero, size_t *length)
{
	size_t	content_length;

	if (fmt->has_precision && fmt->precision == 0 && is_zero)
		digit_length = 0;
	content_length = (size_t)digit_length;
	if (fmt->has_precision && fmt->precision > digit_length)
		content_length = (size_t)fmt->precision;
	if (ft_add_length(&content_length, (size_t)prefix_length) < 0)
		return (-1);
	if ((size_t)fmt->width > content_length)
		content_length = (size_t)fmt->width;
	*length = content_length;
	return (0);
}

static int	ft_measure_string(t_format *fmt, const char *string,
		size_t *length)
{
	size_t	string_length;

	if (string == 0)
		string = "(null)";
	string_length = 0;
	while ((!fmt->has_precision
			|| string_length < (size_t)fmt->precision)
		&& string[string_length])
	{
		if (string_length == (size_t)INT_MAX)
			return (-1);
		string_length++;
	}
	if ((size_t)fmt->width > string_length)
		string_length = (size_t)fmt->width;
	*length = string_length;
	return (0);
}

static int	ft_measure_signed(t_format *fmt, int number, size_t *length)
{
	long			value;
	unsigned long	magnitude;
	int				prefix_length;

	value = (long)number;
	prefix_length = 0;
	if (value < 0)
	{
		prefix_length = 1;
		magnitude = (unsigned long)(-(value + 1)) + 1;
	}
	else
	{
		magnitude = (unsigned long)value;
		if (fmt->flags & (FT_FLAG_PLUS | FT_FLAG_SPACE))
			prefix_length = 1;
	}
	return (ft_measure_numeric(fmt, prefix_length,
			ft_decimal_length(magnitude), magnitude == 0, length));
}

static int	ft_measure_hex(t_format *fmt, unsigned long number,
		int is_pointer, size_t *length)
{
	int	prefix_length;

	prefix_length = 0;
	if (is_pointer || ((fmt->flags & FT_FLAG_HASH) && number != 0))
		prefix_length = 2;
	return (ft_measure_numeric(fmt, prefix_length, ft_hex_length(number),
			number == 0, length));
}

static int	ft_measure_conversion(t_format *fmt, va_list *args,
		size_t *length)
{
	unsigned int	unsigned_number;

	if (fmt->spec == 'c')
	{
		(void)va_arg(*args, int);
		*length = 1;
	}
	else if (fmt->spec == 's')
		return (ft_measure_string(fmt, va_arg(*args, char *), length));
	else if (fmt->spec == 'd' || fmt->spec == 'i')
		return (ft_measure_signed(fmt, va_arg(*args, int), length));
	else if (fmt->spec == 'u')
	{
		unsigned_number = va_arg(*args, unsigned int);
		return (ft_measure_numeric(fmt, 0,
				ft_decimal_length((unsigned long)unsigned_number),
				unsigned_number == 0, length));
	}
	else if (fmt->spec == 'x' || fmt->spec == 'X')
		return (ft_measure_hex(fmt,
				(unsigned long)va_arg(*args, unsigned int), 0, length));
	else if (fmt->spec == 'p')
		return (ft_measure_hex(fmt,
				(unsigned long)(uintptr_t)va_arg(*args, void *), 1, length));
	else
		*length = 1;
	if ((size_t)fmt->width > *length)
		*length = (size_t)fmt->width;
	return (0);
}

int	ft_printf_measure(const char *format, va_list *args)
{
	t_format	fmt;
	size_t		conversion_length;
	size_t		total;

	total = 0;
	while (*format)
	{
		if (*format == '%' && *(format + 1) == '%')
		{
			if (ft_add_length(&total, 1) < 0)
				return (-1);
			format += 2;
		}
		else if (*format == '%')
		{
			format = ft_printf_parse(format + 1, &fmt);
			if (format == 0 || !ft_is_supported_specifier(fmt.spec)
				|| ft_measure_conversion(&fmt, args,
					&conversion_length) < 0
				|| ft_add_length(&total, conversion_length) < 0)
				return (-1);
		}
		else
		{
			if (ft_add_length(&total, 1) < 0)
				return (-1);
			format++;
		}
	}
	return ((int)total);
}
