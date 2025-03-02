#include "push_swap.h"

#include <errno.h>

#ifdef PS_FAULT_INJECTION

typedef union u_allocation_header
{
	struct
	{
		size_t			size;
		unsigned long	magic;
	}	data;
	long double	align_long_double;
	void		*align_pointer;
}	t_allocation_header;

static unsigned long	g_malloc_calls;
static unsigned long	g_live_allocations;

static unsigned long	read_index(const char *name)
{
	const char		*value;
	unsigned long	index;

	value = getenv(name);
	if (value == NULL || *value == '\0')
		return (0);
	index = 0;
	while (*value >= '0' && *value <= '9')
	{
		if (index > (ULONG_MAX - (unsigned long)(*value - '0')) / 10)
			return (0);
		index = index * 10 + (unsigned long)(*value - '0');
		value++;
	}
	if (*value != '\0')
		return (0);
	return (index);
}

static int	at_index(const char *name, unsigned long call)
{
	return (read_index(name) == call && call != 0);
}

#endif

void	*ps_malloc(size_t size)
{
#ifdef PS_FAULT_INJECTION
	t_allocation_header	*header;

	g_malloc_calls++;
	if (at_index("PS_FAIL_MALLOC_AT", g_malloc_calls))
		return (NULL);
	if (size > (size_t)-1 - sizeof(*header))
		return (NULL);
	header = (t_allocation_header *)malloc(sizeof(*header) + size);
	if (header == NULL)
		return (NULL);
	header->data.size = size;
	header->data.magic = 0x50535354UL;
	g_live_allocations++;
	return ((void *)(header + 1));
#else
	return (malloc(size));
#endif
}

void	ps_free(void *pointer)
{
#ifdef PS_FAULT_INJECTION
	t_allocation_header	*header;

	if (pointer == NULL)
		return ;
	header = ((t_allocation_header *)pointer) - 1;
	if (header->data.magic == 0x50535354UL)
	{
		header->data.magic = 0;
		g_live_allocations--;
	}
	free(header);
#else
	free(pointer);
#endif
}

ssize_t	ps_read(int fd, void *buffer, size_t count)
{
	return (read(fd, buffer, count));
}

#ifdef PS_FAULT_INJECTION
static void	raw_report(const char *message)
{
	size_t	length;
	ssize_t	written;

	length = 0;
	while (message[length] != '\0')
		length++;
	while (length > 0)
	{
		written = write(2, message, length);
		if (written < 0 && errno == EINTR)
			continue ;
		if (written <= 0)
			return ;
		message += written;
		length -= (size_t)written;
	}
}
#endif

int	ps_test_finish(int status)
{
#ifdef PS_FAULT_INJECTION
	if (getenv("PS_REPORT_ALLOCATIONS") != NULL)
	{
		if (g_live_allocations == 0)
			raw_report("PS_LIVE_ALLOCATIONS=0\n");
		else
		{
			raw_report("PS_LIVE_ALLOCATIONS=NONZERO\n");
			return (99);
		}
	}
#endif
	return (status);
}
