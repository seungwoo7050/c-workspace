#include "test.h"

#include "libft.h"

#include <stddef.h>
#include <string.h>

#define COPY_BUFFER_SIZE 128

static void	seed_copy_buffers(unsigned char *destination,
		unsigned char *source)
{
	size_t	index;

	index = 0;
	while (index < COPY_BUFFER_SIZE)
	{
		destination[index] = (unsigned char)(index * 19U + 7U);
		source[index] = (unsigned char)(index * 43U + 3U);
		index++;
	}
}

static void	check_copy(size_t destination_offset, size_t source_offset,
		size_t length)
{
	unsigned char	actual[COPY_BUFFER_SIZE];
	unsigned char	expected[COPY_BUFFER_SIZE];
	unsigned char	source[COPY_BUFFER_SIZE];
	unsigned char	source_before[COPY_BUFFER_SIZE];

	seed_copy_buffers(actual, source);
	memcpy(expected, actual, sizeof(actual));
	memcpy(source_before, source, sizeof(source));
	CHECK(ft_memcpy(actual + destination_offset, source + source_offset, length)
		== actual + destination_offset);
	memcpy(expected + destination_offset, source + source_offset, length);
	CHECK(memcmp(actual, expected, sizeof(actual)) == 0);
	CHECK(memcmp(source, source_before, sizeof(source)) == 0);
}

void	test_memory_copy(void)
{
	static const size_t	lengths[] = {0, 1, 2, 3, 7, 8, 15, 16, 31, 64};
	size_t			index;

	CHECK(ft_memcpy(NULL, NULL, 0) == NULL);
	index = 0;
	while (index < sizeof(lengths) / sizeof(lengths[0]))
	{
		check_copy(0, 0, lengths[index]);
		check_copy(1, 3, lengths[index]);
		check_copy(9, 5, lengths[index]);
		index++;
	}
}
