#ifndef FT_PRINTF_INTERNAL_H
# define FT_PRINTF_INTERNAL_H

# include "ft_printf.h"

# include <stdarg.h>
# include <stddef.h>

# define FT_FLAG_LEFT 1
# define FT_FLAG_ZERO 2
# define FT_FLAG_HASH 4
# define FT_FLAG_SPACE 8
# define FT_FLAG_PLUS 16

typedef struct s_printf
{
	int	fd;
	int	count;
	int	error;
}	t_printf;

typedef struct s_format
{
	int		flags;
	int		width;
	int		precision;
	int		has_precision;
	char	spec;
}	t_format;

void		ft_printf_init(t_printf *ctx, int fd);
int			ft_printf_write(t_printf *ctx, const char *buffer, size_t length);
int			ft_printf_putchar(t_printf *ctx, char c);
int			ft_printf_putnchar(t_printf *ctx, char c, int length);
int			ft_printf_write_numeric_layout(t_printf *ctx, t_format *fmt,
				const char *prefix, const char *digits, int digit_len,
				int is_zero);
void		ft_printf_init_format(t_format *fmt);
const char	*ft_printf_parse(const char *format, t_format *fmt);
int			ft_printf_dispatch(t_printf *ctx, t_format *fmt, va_list *args);
int			ft_printf_print_char(t_printf *ctx, t_format *fmt, int c);
int			ft_printf_print_string(t_printf *ctx, t_format *fmt,
				const char *string);
int			ft_printf_print_percent(t_printf *ctx, t_format *fmt);
int			ft_printf_print_signed(t_printf *ctx, t_format *fmt, int number);
int			ft_printf_print_unsigned(t_printf *ctx, t_format *fmt,
				unsigned int number);
int			ft_printf_print_hex(t_printf *ctx, t_format *fmt,
				unsigned int number);
int			ft_printf_print_pointer(t_printf *ctx, t_format *fmt,
				void *pointer);

#endif
