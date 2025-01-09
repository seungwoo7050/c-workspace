#include "libft.h"

void	*ft_memmove(void *destination, const void *source, size_t length)
{
	unsigned char		*destination_byte;
	const unsigned char	*source_byte;
	size_t			offset;

	destination_byte = destination;
	source_byte = source;
	if (destination_byte == source_byte || length == 0)
		return (destination);
	offset = 1;
	while (offset < length)
	{
		if (destination_byte == source_byte + offset)
		{
			while (length > 0)
			{
				length--;
				destination_byte[length] = source_byte[length];
			}
			return (destination);
		}
		offset++;
	}
	ft_memcpy(destination_byte, source_byte, length);
	return (destination);
}
