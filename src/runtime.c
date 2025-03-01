#include "push_swap.h"

void	*ps_malloc(size_t size)
{
	return (malloc(size));
}

void	ps_free(void *pointer)
{
	free(pointer);
}

ssize_t	ps_read(int fd, void *buffer, size_t count)
{
	return (read(fd, buffer, count));
}
