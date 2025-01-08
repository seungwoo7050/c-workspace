#include "test.h"

#include "libft.h"

#include <stddef.h>
#include <string.h>

#define BUFFER_SIZE 96

static void	seed_buffers(unsigned char *actual, unsigned char *expected)
{
	size_t	index;

	index = 0;
	while (index < BUFFER_SIZE)
	{
		actual[index] = (unsigned char)(index * 37U + 11U);
		expected[index] = actual[index];
		index++;
	}
}

static void	check_memset(size_t offset, size_t length, int byte)
{
	unsigned char	actual[BUFFER_SIZE];
	unsigned char	expected[BUFFER_SIZE];

	seed_buffers(actual, expected);
	CHECK(ft_memset(actual + offset, byte, length) == actual + offset);
	memset(expected + offset, byte, length);
	CHECK(memcmp(actual, expected, BUFFER_SIZE) == 0);
}

static void	check_bzero(size_t offset, size_t length)
{
	unsigned char	actual[BUFFER_SIZE];
	unsigned char	expected[BUFFER_SIZE];

	seed_buffers(actual, expected);
	ft_bzero(actual + offset, length);
	memset(expected + offset, 0, length);
	CHECK(memcmp(actual, expected, BUFFER_SIZE) == 0);
}

void	test_memory_fill(void)
{
	static const size_t	offsets[] = {0, 1, 3, 7};
	static const size_t	lengths[] = {0, 1, 2, 3, 7, 8, 15, 16, 31, 32, 63};
	static const int	bytes[] = {0, 1, 127, 128, 255, 256, -1};
	size_t			offset_index;
	size_t			length_index;
	size_t			byte_index;

	CHECK(ft_memset(NULL, 0, 0) == NULL);
	ft_bzero(NULL, 0);
	offset_index = 0;
	while (offset_index < sizeof(offsets) / sizeof(offsets[0]))
	{
		length_index = 0;
		while (length_index < sizeof(lengths) / sizeof(lengths[0]))
		{
			byte_index = 0;
			while (byte_index < sizeof(bytes) / sizeof(bytes[0]))
			{
				check_memset(offsets[offset_index], lengths[length_index],
					bytes[byte_index]);
				byte_index++;
			}
			check_bzero(offsets[offset_index], lengths[length_index]);
			length_index++;
		}
		offset_index++;
	}
}
