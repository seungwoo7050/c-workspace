#include "test.h"

#include "libft.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

static void	check_calloc(size_t count, size_t size)
{
	unsigned char	*allocation;
	size_t		bytes;
	size_t		index;

	allocation = ft_calloc(count, size);
	CHECK(allocation != NULL);
	bytes = count * size;
	if (bytes == 0)
		bytes = 1;
	index = 0;
	while (allocation != NULL && index < bytes)
	{
		CHECK(allocation[index] == 0);
		index++;
	}
	free(allocation);
}

static void	check_strdup(const char *text)
{
	char	*duplicate;

	duplicate = ft_strdup(text);
	CHECK(duplicate != NULL);
	if (duplicate != NULL)
	{
		CHECK(strcmp(duplicate, text) == 0);
		CHECK(duplicate != text);
		free(duplicate);
	}
}

void	test_allocate(void)
{
	check_calloc(0, 0);
	check_calloc(0, 8);
	check_calloc(8, 0);
	check_calloc(1, 1);
	check_calloc(7, 9);
	check_calloc(32, 4);
	CHECK(ft_calloc((size_t)-1, 2) == NULL);
	CHECK(ft_calloc((size_t)-1 / 3 + 1, 3) == NULL);
	check_strdup("");
	check_strdup("a");
	check_strdup("foundation ownership");
	CHECK(ft_strdup(NULL) == NULL);
}
