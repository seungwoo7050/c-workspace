#include "test.h"

#include "libft.h"

#include <stddef.h>
#include <string.h>

#define STRING_BUFFER_SIZE 40

static size_t	reference_strlcpy(char *destination, const char *source,
		size_t capacity)
{
	size_t	length;
	size_t	index;

	length = strlen(source);
	if (capacity == 0)
		return (length);
	index = 0;
	while (index + 1 < capacity && source[index] != '\0')
	{
		destination[index] = source[index];
		index++;
	}
	destination[index] = '\0';
	return (length);
}

static size_t	reference_strlcat(char *destination, const char *source,
		size_t capacity)
{
	size_t	destination_length;
	size_t	source_length;
	size_t	index;

	destination_length = 0;
	while (destination_length < capacity
		&& destination[destination_length] != '\0')
		destination_length++;
	source_length = strlen(source);
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

static void	seed_string_buffer(char *buffer)
{
	size_t	index;

	index = 0;
	while (index < STRING_BUFFER_SIZE)
	{
		buffer[index] = (char)('A' + index % 26);
		index++;
	}
}

static void	check_strlcpy(const char *source, size_t capacity)
{
	char	actual[STRING_BUFFER_SIZE];
	char	expected[STRING_BUFFER_SIZE];
	size_t	actual_length;
	size_t	expected_length;

	seed_string_buffer(actual);
	memcpy(expected, actual, sizeof(actual));
	actual_length = ft_strlcpy(actual, source, capacity);
	expected_length = reference_strlcpy(expected, source, capacity);
	CHECK(actual_length == expected_length);
	CHECK(memcmp(actual, expected, sizeof(actual)) == 0);
}

static void	check_strlcat(const char *initial, const char *source,
		size_t capacity)
{
	char	actual[STRING_BUFFER_SIZE];
	char	expected[STRING_BUFFER_SIZE];
	size_t	initial_length;
	size_t	actual_length;
	size_t	expected_length;

	seed_string_buffer(actual);
	initial_length = strlen(initial);
	memcpy(actual, initial, initial_length + 1);
	memcpy(expected, actual, sizeof(actual));
	actual_length = ft_strlcat(actual, source, capacity);
	expected_length = reference_strlcat(expected, source, capacity);
	CHECK(actual_length == expected_length);
	CHECK(memcmp(actual, expected, sizeof(actual)) == 0);
}

void	test_string_bounds(void)
{
	static const char	*texts[] = {"", "a", "hello", "embedded", "0123456789"};
	size_t			text_index;
	size_t			capacity;

	text_index = 0;
	while (text_index < sizeof(texts) / sizeof(texts[0]))
	{
		CHECK(ft_strlen(texts[text_index]) == strlen(texts[text_index]));
		capacity = 0;
		while (capacity <= 16)
		{
			check_strlcpy(texts[text_index], capacity);
			check_strlcat("", texts[text_index], capacity);
			check_strlcat("abc", texts[text_index], capacity);
			check_strlcat("abcdefgh", texts[text_index], capacity);
			capacity++;
		}
		text_index++;
	}
}
