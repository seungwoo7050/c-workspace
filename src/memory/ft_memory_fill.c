#include "libft.h"

void	*ft_memset(void *memory, int byte, size_t length)
{
	unsigned char	*byte_pointer;

	byte_pointer = memory;
	while (length > 0)
	{
		*byte_pointer = (unsigned char)byte;
		byte_pointer++;
		length--;
	}
	return (memory);
}

void	ft_bzero(void *memory, size_t length)
{
	ft_memset(memory, 0, length);
}
