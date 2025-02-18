#include "push_swap.h"

size_t	ps_strlen(const char *str)
{
	size_t	len;

	len = 0;
	while (str[len] != '\0')
		len++;
	return (len);
}

int	ps_strcmp(const char *a, const char *b)
{
	size_t	i;

	i = 0;
	while (a[i] != '\0' && b[i] != '\0' && a[i] == b[i])
		i++;
	return ((unsigned char)a[i] - (unsigned char)b[i]);
}

void	ps_putstr_fd(int fd, const char *str)
{
	if (str != NULL)
		(void)write(fd, str, ps_strlen(str));
}

void	write_error(void)
{
	ps_putstr_fd(2, "Error\n");
}
