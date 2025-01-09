#include "libft.h"

void	*ft_memchr(const void *memory, int byte, size_t length)
{
	const unsigned char	*memory_byte;
	size_t			index;

	memory_byte = memory;
	index = 0;
	while (index < length)
	{
		if (memory_byte[index] == (unsigned char)byte)
			return ((void *)(memory_byte + index));
		index++;
	}
	return (NULL);
}

int	ft_memcmp(const void *left, const void *right, size_t length)
{
	const unsigned char	*left_byte;
	const unsigned char	*right_byte;
	size_t			index;

	left_byte = left;
	right_byte = right;
	index = 0;
	while (index < length)
	{
		if (left_byte[index] != right_byte[index])
			return ((int)left_byte[index] - (int)right_byte[index]);
		index++;
	}
	return (0);
}
