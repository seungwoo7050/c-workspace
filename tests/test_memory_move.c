#include "test.h"

#include "libft.h"

#include <stddef.h>
#include <string.h>

#define MOVE_BUFFER_SIZE 128

static void	seed_move_buffer(unsigned char *buffer)
{
	size_t	index;

	index = 0;
	while (index < MOVE_BUFFER_SIZE)
	{
		buffer[index] = (unsigned char)(index * 29U + 17U);
		index++;
	}
}

static void	check_move(size_t destination_offset, size_t source_offset,
		size_t length)
{
	unsigned char	actual[MOVE_BUFFER_SIZE];
	unsigned char	expected[MOVE_BUFFER_SIZE];

	seed_move_buffer(actual);
	memcpy(expected, actual, sizeof(actual));
	CHECK(ft_memmove(actual + destination_offset, actual + source_offset, length)
		== actual + destination_offset);
	memmove(expected + destination_offset, expected + source_offset, length);
	CHECK(memcmp(actual, expected, sizeof(actual)) == 0);
}

void	test_memory_move(void)
{
	static const size_t	lengths[] = {0, 1, 2, 3, 7, 8, 15, 16, 31, 63};
	size_t			index;

	CHECK(ft_memmove(NULL, NULL, 0) == NULL);
	index = 0;
	while (index < sizeof(lengths) / sizeof(lengths[0]))
	{
		check_move(0, 0, lengths[index]);
		check_move(0, 1, lengths[index]);
		check_move(1, 0, lengths[index]);
		check_move(7, 19, lengths[index]);
		check_move(23, 5, lengths[index]);
		index++;
	}
}
