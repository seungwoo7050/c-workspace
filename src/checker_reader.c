#include "push_swap.h"

static int	grow_line(char **line, size_t *capacity, size_t needed)
{
	char	*next;
	size_t	new_capacity;
	size_t	i;

	if (*capacity > needed)
		return (1);
	new_capacity = 32;
	if (*capacity != 0)
		new_capacity = *capacity;
	while (new_capacity <= needed)
		new_capacity *= 2;
	next = (char *)malloc(new_capacity);
	if (next == NULL)
		return (0);
	i = 0;
	while (i < *capacity && *line != NULL)
	{
		next[i] = (*line)[i];
		i++;
	}
	free(*line);
	*line = next;
	*capacity = new_capacity;
	return (1);
}

int	read_next_line(int fd, char **line)
{
	char	c;
	ssize_t	bytes;
	size_t	len;
	size_t	capacity;

	*line = NULL;
	len = 0;
	capacity = 0;
	while (1)
	{
		bytes = read(fd, &c, 1);
		if (bytes < 0)
			return (free(*line), -1);
		if (bytes == 0)
			break ;
		if (c == '\n')
			break ;
		if (!grow_line(line, &capacity, len + 1))
			return (free(*line), -1);
		(*line)[len++] = c;
	}
	if (bytes == 0 && len == 0)
		return (free(*line), 0);
	if (!grow_line(line, &capacity, len + 1))
		return (free(*line), -1);
	(*line)[len] = '\0';
	return (1);
}
