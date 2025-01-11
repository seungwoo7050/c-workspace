#include "libft.h"

#include <stdlib.h>

static size_t	count_fields(const char *text, char delimiter)
{
	size_t	count;
	size_t	index;

	count = 0;
	index = 0;
	while (text[index] != '\0')
	{
		while (text[index] == delimiter && text[index] != '\0')
			index++;
		if (text[index] != '\0')
			count++;
		while (text[index] != delimiter && text[index] != '\0')
			index++;
	}
	return (count);
}

static char	*copy_field(const char *start, size_t length)
{
	char	*field;

	field = malloc(length + 1);
	if (field == NULL)
		return (NULL);
	ft_memcpy(field, start, length);
	field[length] = '\0';
	return (field);
}

static void	free_fields(char **fields, size_t count)
{
	while (count > 0)
	{
		count--;
		free(fields[count]);
	}
	free(fields);
}

char	**ft_split(const char *text, char delimiter)
{
	char	**fields;
	size_t	text_index;
	size_t	field_index;
	size_t	start;

	if (text == NULL)
		return (NULL);
	fields = ft_calloc(count_fields(text, delimiter) + 1, sizeof(char *));
	if (fields == NULL)
		return (NULL);
	text_index = 0;
	field_index = 0;
	while (text[text_index] != '\0')
	{
		while (text[text_index] == delimiter && text[text_index] != '\0')
			text_index++;
		start = text_index;
		while (text[text_index] != delimiter && text[text_index] != '\0')
			text_index++;
		if (text_index > start)
		{
			fields[field_index] = copy_field(text + start, text_index - start);
			if (fields[field_index] == NULL)
				return (free_fields(fields, field_index), NULL);
			field_index++;
		}
	}
	return (fields);
}
