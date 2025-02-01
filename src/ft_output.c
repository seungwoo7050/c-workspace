#include "ft_printf_internal.h"

#include <errno.h>
#include <limits.h>
#include <unistd.h>

#ifdef FT_PRINTF_TEST_WRITE
ssize_t	ft_printf_test_write(int fd, const void *buffer, size_t length);
# define FT_PRINTF_SYSTEM_WRITE ft_printf_test_write
#else
# define FT_PRINTF_SYSTEM_WRITE write
#endif

void	ft_printf_init(t_printf *ctx, int fd)
{
	ctx->fd = fd;
	ctx->count = 0;
	ctx->error = 0;
}

int	ft_printf_write(t_printf *ctx, const char *buffer, size_t length)
{
	ssize_t	written;
	size_t	request;

	if (ctx->error)
		return (-1);
	while (length > 0)
	{
		request = length;
		if (request > (size_t)SSIZE_MAX)
			request = (size_t)SSIZE_MAX;
		written = FT_PRINTF_SYSTEM_WRITE(ctx->fd, buffer, request);
		if (written < 0 && errno == EINTR)
			continue ;
		if (written <= 0)
		{
			ctx->error = 1;
			return (-1);
		}
		if (written > INT_MAX || ctx->count > INT_MAX - (int)written)
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
	char	buffer[64];
	int		index;
	int		chunk;

	index = 0;
	while (index < (int)sizeof(buffer))
		buffer[index++] = c;
	while (length > 0)
	{
		chunk = length;
		if (chunk > (int)sizeof(buffer))
			chunk = (int)sizeof(buffer);
		if (ft_printf_write(ctx, buffer, (size_t)chunk) < 0)
			return (-1);
		length -= chunk;
	}
	return (0);
}
