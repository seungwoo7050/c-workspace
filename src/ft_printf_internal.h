#ifndef FT_PRINTF_INTERNAL_H
# define FT_PRINTF_INTERNAL_H

# include "ft_printf.h"

# include <stddef.h>

typedef struct s_printf
{
	int	fd;
	int	count;
	int	error;
}	t_printf;

void		ft_printf_init(t_printf *ctx, int fd);
int			ft_printf_write(t_printf *ctx, const char *buffer, size_t length);
int			ft_printf_putchar(t_printf *ctx, char c);

#endif
