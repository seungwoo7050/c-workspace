#include "test.h"

#include "libft.h"

#include <stdlib.h>
#include <string.h>

static unsigned int	g_map_calls;
static unsigned int	g_iter_calls;

static char	map_character(unsigned int index, char character)
{
	CHECK(index == g_map_calls);
	g_map_calls++;
	return ((char)(character + (char)index));
}

static void	iterate_character(unsigned int index, char *character)
{
	CHECK(index == g_iter_calls);
	g_iter_calls++;
	*character = (char)(*character - (char)index);
}

static void	erase_character(unsigned int index, char *character)
{
	CHECK(index == g_iter_calls);
	g_iter_calls++;
	*character = '\0';
}

static void	check_map(const char *text, const char *expected)
{
	char	*mapped;

	g_map_calls = 0;
	mapped = ft_strmapi(text, map_character);
	CHECK(mapped != NULL);
	if (mapped != NULL)
	{
		CHECK(strcmp(mapped, expected) == 0);
		CHECK(mapped != text);
		CHECK(g_map_calls == strlen(text));
		free(mapped);
	}
}

static void	check_iteration(char *text, const char *expected)
{
	size_t	initial_length;

	initial_length = strlen(text);
	g_iter_calls = 0;
	ft_striteri(text, iterate_character);
	CHECK(strcmp(text, expected) == 0);
	CHECK(g_iter_calls == initial_length);
}

void	test_string_transform(void)
{
	char	first[] = "aceg";
	char	second[] = "bdfh";
	char	erased[] = "fixed";

	check_map("", "");
	check_map("aaaa", "abcd");
	check_map("0123", "0246");
	check_iteration(first, "abcd");
	check_iteration(second, "bcde");
	g_iter_calls = 0;
	ft_striteri(erased, erase_character);
	CHECK(g_iter_calls == 5);
	CHECK(erased[0] == '\0' && erased[1] == '\0' && erased[2] == '\0');
	CHECK(erased[3] == '\0' && erased[4] == '\0' && erased[5] == '\0');
	CHECK(ft_strmapi(NULL, map_character) == NULL);
	CHECK(ft_strmapi("text", NULL) == NULL);
	ft_striteri(NULL, iterate_character);
	ft_striteri(first, NULL);
}
