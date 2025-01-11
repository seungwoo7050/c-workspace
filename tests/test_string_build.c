#include "test.h"

#include "libft.h"

#include <stdlib.h>
#include <string.h>

static void	check_substr(const char *text, unsigned int start, size_t length,
		const char *expected)
{
	char	*substring;

	substring = ft_substr(text, start, length);
	CHECK(substring != NULL);
	if (substring != NULL)
	{
		CHECK(strcmp(substring, expected) == 0);
		CHECK(substring != text);
		free(substring);
	}
}

static void	check_join(const char *left, const char *right,
		const char *expected)
{
	char	*joined;

	joined = ft_strjoin(left, right);
	CHECK(joined != NULL);
	if (joined != NULL)
	{
		CHECK(strcmp(joined, expected) == 0);
		CHECK(joined != left && joined != right);
		free(joined);
	}
}

static void	check_trim(const char *text, const char *set,
		const char *expected)
{
	char	*trimmed;

	trimmed = ft_strtrim(text, set);
	CHECK(trimmed != NULL);
	if (trimmed != NULL)
	{
		CHECK(strcmp(trimmed, expected) == 0);
		CHECK(trimmed != text);
		free(trimmed);
	}
}

void	test_string_build(void)
{
	check_substr("", 0, 0, "");
	check_substr("foundation", 0, 4, "foun");
	check_substr("foundation", 4, 99, "dation");
	check_substr("foundation", 10, 4, "");
	check_substr("foundation", 99, 4, "");
	check_substr("foundation", 3, 0, "");
	CHECK(ft_substr(NULL, 0, 1) == NULL);
	check_join("", "", "");
	check_join("left", "", "left");
	check_join("", "right", "right");
	check_join("left", "right", "leftright");
	CHECK(ft_strjoin(NULL, "right") == NULL);
	CHECK(ft_strjoin("left", NULL) == NULL);
	check_trim("", "", "");
	check_trim("hello", "", "hello");
	check_trim("  hello  ", " ", "hello");
	check_trim("abbahelloabba", "ab", "hello");
	check_trim("aaaa", "a", "");
	check_trim("abc", "xyz", "abc");
	CHECK(ft_strtrim(NULL, "x") == NULL);
	CHECK(ft_strtrim("x", NULL) == NULL);
}
