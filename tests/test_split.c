#include "test.h"

#include "libft.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

static void	free_split(char **fields)
{
	size_t	index;

	index = 0;
	while (fields[index] != NULL)
	{
		free(fields[index]);
		index++;
	}
	free(fields);
}

static void	check_split(const char *text, char delimiter,
		const char **expected, size_t expected_count)
{
	char	**fields;
	size_t	index;

	fields = ft_split(text, delimiter);
	CHECK(fields != NULL);
	if (fields == NULL)
		return ;
	index = 0;
	while (index < expected_count)
	{
		CHECK(fields[index] != NULL);
		if (fields[index] != NULL)
		{
			CHECK(strcmp(fields[index], expected[index]) == 0);
			CHECK(fields[index] != text);
		}
		index++;
	}
	CHECK(fields[expected_count] == NULL);
	free_split(fields);
}

void	test_split(void)
{
	static const char	*none[] = {NULL};
	static const char	*one[] = {"single"};
	static const char	*three[] = {"alpha", "beta", "gamma"};
	static const char	*spaced[] = {"one", "two", "three"};
	const char		*long_expected[1];
	char			long_token[1025];
	size_t			index;

	index = 0;
	while (index + 1 < sizeof(long_token))
	{
		long_token[index] = 'x';
		index++;
	}
	long_token[index] = '\0';
	long_expected[0] = long_token;

	check_split("", ',', none, 0);
	check_split(",,,,", ',', none, 0);
	check_split("single", ',', one, 1);
	check_split(",single,", ',', one, 1);
	check_split("alpha,beta,gamma", ',', three, 3);
	check_split(",,alpha,,,beta,gamma,,", ',', three, 3);
	check_split(" one  two   three ", ' ', spaced, 3);
	check_split("single", '\0', one, 1);
	check_split("", '\0', none, 0);
	check_split(long_token, ',', long_expected, 1);
	CHECK(ft_split(NULL, ',') == NULL);
}
