#include "libft.h"

#include <unistd.h>

void	ft_putchar_fd(char character, int fd)
{
	(void)write(fd, &character, 1);
}

void	ft_putstr_fd(char *text, int fd)
{
	(void)write(fd, text, ft_strlen(text));
}

void	ft_putendl_fd(char *text, int fd)
{
	ft_putstr_fd(text, fd);
	ft_putchar_fd('\n', fd);
}

static void	put_unsigned(unsigned int magnitude, int fd)
{
	if (magnitude >= 10U)
		put_unsigned(magnitude / 10U, fd);
	ft_putchar_fd((char)('0' + magnitude % 10U), fd);
}

void	ft_putnbr_fd(int number, int fd)
{
	unsigned int	magnitude;

	if (number < 0)
	{
		ft_putchar_fd('-', fd);
		magnitude = (unsigned int)(-(number + 1)) + 1U;
	}
	else
		magnitude = (unsigned int)number;
	put_unsigned(magnitude, fd);
}
