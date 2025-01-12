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

void	ft_putnbr_fd(int number, int fd)
{
	char		buffer[11];
	unsigned int	magnitude;
	size_t		index;

	if (number < 0)
		magnitude = (unsigned int)(-(number + 1)) + 1U;
	else
		magnitude = (unsigned int)number;
	index = sizeof(buffer);
	while (magnitude >= 10U)
	{
		index--;
		buffer[index] = (char)('0' + magnitude % 10U);
		magnitude /= 10U;
	}
	index--;
	buffer[index] = (char)('0' + magnitude);
	if (number < 0)
	{
		index--;
		buffer[index] = '-';
	}
	(void)write(fd, buffer + index, sizeof(buffer) - index);
}
