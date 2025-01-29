#include "ft_printf_internal.h"

#include <limits.h>
#include <unistd.h>

void	ft_printf_init(t_printf *ctx, int fd)
{
	ctx->fd = fd;
	ctx->count = 0;
	ctx->error = 0;
}

int	ft_printf_write(t_printf *ctx, const char *buffer, size_t length)
{
	ssize_t	written;

	if (ctx->error)
		return (-1);
	while (length > 0)
	{
		written = write(ctx->fd, buffer, length);
		if (written <= 0)
		{
			ctx->error = 1;
			return (-1);
		}
		if (ctx->count > INT_MAX - (int)written)
		{
			ctx->error = 1;
			return (-1);
		}
		ctx->count += (int)written;
		buffer += written;
		length -= (size_t)written;
	}
	return (0);
}

int	ft_printf_putchar(t_printf *ctx, char c)
{
	return (ft_printf_write(ctx, &c, 1));
}

int	ft_printf_putnchar(t_printf *ctx, char c, int length)
{
	while (length > 0)
	{
		if (ft_printf_putchar(ctx, c) < 0)
			return (-1);
		length--;
	}
	return (0);
}
