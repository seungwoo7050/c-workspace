#ifndef FT_PRINTF_INTERNAL_H
# define FT_PRINTF_INTERNAL_H

# include "ft_printf.h"

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
void		ft_printf_init_format(t_format *fmt);
const char	*ft_printf_parse(const char *format, t_format *fmt);

#endif
