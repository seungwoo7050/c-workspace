#include "push_swap.h"

#include <errno.h>

int	read_next_line(int fd, char **line)
{
	char	c;
	ssize_t	bytes;
	size_t	len;

	*line = (char *)ps_malloc(PS_COMMAND_MAX + 1);
	if (*line == NULL)
		return (-1);
	len = 0;
	while (1)
	{
		bytes = ps_read(fd, &c, 1);
		if (bytes < 0 && errno == EINTR)
			continue ;
		if (bytes < 0)
			return (ps_free(*line), *line = NULL, -1);
		if (bytes == 0)
			break ;
		if (c == '\n')
			break ;
		if (c == '\0' || len >= PS_COMMAND_MAX)
			return (ps_free(*line), *line = NULL, -1);
		(*line)[len++] = c;
	}
	if (bytes == 0 && len == 0)
		return (ps_free(*line), *line = NULL, 0);
	(*line)[len] = '\0';
	return (1);
}
