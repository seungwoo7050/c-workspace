#define _POSIX_C_SOURCE 200809L

#include "libft.h"

#include <errno.h>
#include <limits.h>
#include <unistd.h>

static int	write_all(int fd, const char *buffer, size_t length)
{
	ssize_t	written;
	size_t	offset;
	size_t	request;

	offset = 0;
	while (offset < length)
	{
		request = length - offset;
		if (request > (size_t)SSIZE_MAX)
			request = (size_t)SSIZE_MAX;
		written = write(fd, buffer + offset, request);
		if (written > 0)
			offset += (size_t)written;
		else if (written < 0 && errno == EINTR)
			continue ;
		else
		{
			if (written == 0)
				errno = EIO;
			return (0);
		}
	}
	return (1);
}

void	ft_putchar_fd(char character, int fd)
{
	(void)write_all(fd, &character, 1);
}

void	ft_putstr_fd(char *text, int fd)
{
	(void)write_all(fd, text, ft_strlen(text));
}

void	ft_putendl_fd(char *text, int fd)
{
	if (write_all(fd, text, ft_strlen(text)))
		(void)write_all(fd, "\n", 1);
}

static int	put_unsigned(unsigned int magnitude, int fd)
{
	char	digit;

	if (magnitude >= 10U && !put_unsigned(magnitude / 10U, fd))
		return (0);
	digit = (char)('0' + magnitude % 10U);
	return (write_all(fd, &digit, 1));
}

void	ft_putnbr_fd(int number, int fd)
{
	unsigned int	magnitude;

	if (number < 0)
	{
		if (!write_all(fd, "-", 1))
			return ;
		magnitude = (unsigned int)(-(number + 1)) + 1U;
	}
	else
		magnitude = (unsigned int)number;
	(void)put_unsigned(magnitude, fd);
}
