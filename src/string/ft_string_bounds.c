#include "libft.h"

size_t	ft_strlen(const char *text)
{
	size_t	length;

	length = 0;
	while (text[length] != '\0')
		length++;
	return (length);
}

size_t	ft_strlcpy(char *destination, const char *source, size_t capacity)
{
	size_t	source_length;
	size_t	index;

	source_length = ft_strlen(source);
	if (capacity == 0)
		return (source_length);
	index = 0;
	while (source[index] != '\0' && index + 1 < capacity)
	{
		destination[index] = source[index];
		index++;
	}
	destination[index] = '\0';
	return (source_length);
}

size_t	ft_strlcat(char *destination, const char *source, size_t capacity)
{
	size_t	destination_length;
	size_t	source_length;
	size_t	index;

	destination_length = 0;
	while (destination_length < capacity
		&& destination[destination_length] != '\0')
		destination_length++;
	source_length = ft_strlen(source);
	if (destination_length == capacity)
		return (capacity + source_length);
	index = 0;
	while (source[index] != '\0'
		&& destination_length + index + 1 < capacity)
	{
		destination[destination_length + index] = source[index];
		index++;
	}
	destination[destination_length + index] = '\0';
	return (destination_length + source_length);
}
