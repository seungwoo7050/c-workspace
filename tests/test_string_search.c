#include "test.h"

#include "libft.h"

#include <stddef.h>
#include <string.h>

static int	search_sign(int value)
{
	if (value < 0)
		return (-1);
	if (value > 0)
		return (1);
	return (0);
}

static char	*reference_strnstr(const char *haystack, const char *needle,
		size_t length)
{
	size_t	haystack_index;
	size_t	needle_index;

	if (*needle == '\0')
		return ((char *)haystack);
	haystack_index = 0;
	while (haystack_index < length && haystack[haystack_index] != '\0')
	{
		needle_index = 0;
		while (needle[needle_index] != '\0'
			&& needle_index < length - haystack_index
			&& haystack[haystack_index + needle_index]
				== needle[needle_index])
			needle_index++;
		if (needle[needle_index] == '\0')
			return ((char *)(haystack + haystack_index));
		haystack_index++;
	}
	return (NULL);
}

static void	check_character_search(const char *text, int character)
{
	CHECK(ft_strchr(text, character) == strchr(text, character));
	CHECK(ft_strrchr(text, character) == strrchr(text, character));
}

static void	check_bounded_compare(const char *left, const char *right)
{
	size_t	length;

	length = 0;
	while (length <= 12)
	{
		CHECK(search_sign(ft_strncmp(left, right, length))
			== search_sign(strncmp(left, right, length)));
		length++;
	}
}

static void	check_substring(const char *haystack, const char *needle)
{
	size_t	length;

	length = 0;
	while (length <= strlen(haystack) + 2)
	{
		CHECK(ft_strnstr(haystack, needle, length)
			== reference_strnstr(haystack, needle, length));
		length++;
	}
}

void	test_string_search(void)
{
	check_character_search("", '\0');
	check_character_search("banana", 'a');
	check_character_search("banana", 'b');
	check_character_search("banana", 'n');
	check_character_search("banana", 'z');
	check_character_search("banana", '\0');
	check_character_search("banana", 256);
	check_bounded_compare("", "");
	check_bounded_compare("abc", "abc");
	check_bounded_compare("abc", "abd");
	check_bounded_compare("abd", "abc");
	check_bounded_compare("abc", "ab");
	check_bounded_compare("ab", "abc");
	check_substring("", "");
	check_substring("abc", "");
	check_substring("abc", "a");
	check_substring("abc", "bc");
	check_substring("abc", "abcd");
	check_substring("aaaaab", "aaab");
	check_substring("prefix middle suffix", "middle");
	check_substring("prefix middle suffix", "missing");
}
