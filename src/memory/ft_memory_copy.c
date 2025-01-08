#include "libft.h"

void	*ft_memcpy(void *destination, const void *source, size_t length)
{
	unsigned char		*destination_byte;
	const unsigned char	*source_byte;

	destination_byte = destination;
	source_byte = source;
	while (length > 0)
	{
		*destination_byte = *source_byte;
		destination_byte++;
		source_byte++;
		length--;
	}
	return (destination);
}
