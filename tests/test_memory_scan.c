#include "test.h"

#include "libft.h"

#include <stddef.h>
#include <string.h>

static int	sign_of(int value)
{
	if (value < 0)
		return (-1);
	if (value > 0)
		return (1);
	return (0);
}

static void	check_memchr(const unsigned char *bytes, size_t length, int byte)
{
	CHECK(ft_memchr(bytes, byte, length) == memchr(bytes, byte, length));
}

static void	check_memcmp(const unsigned char *left,
		const unsigned char *right, size_t length)
{
	CHECK(sign_of(ft_memcmp(left, right, length))
		== sign_of(memcmp(left, right, length)));
}

void	test_memory_scan(void)
{
	unsigned char	left[] = {0, 1, 2, 0, 127, 128, 254, 255, 2, 1};
	unsigned char	right[] = {0, 1, 2, 0, 127, 129, 254, 255, 2, 1};
	size_t		length;

	CHECK(ft_memchr(NULL, 0, 0) == NULL);
	CHECK(ft_memcmp(NULL, NULL, 0) == 0);
	length = 0;
	while (length <= sizeof(left))
	{
		check_memchr(left, length, 0);
		check_memchr(left, length, 2);
		check_memchr(left, length, 255);
		check_memchr(left, length, 256);
		check_memchr(left, length, -1);
		check_memchr(left, length, 42);
		check_memcmp(left, left, length);
		check_memcmp(left, right, length);
		check_memcmp(right, left, length);
		length++;
	}
}
