#include "test.h"

#include "libft.h"

#include <ctype.h>
#include <limits.h>
#include <locale.h>
#include <stdio.h>

void	test_char(void)
{
	int	value;

	CHECK(setlocale(LC_CTYPE, "C") != NULL);
	value = 0;
	while (value <= UCHAR_MAX)
	{
		CHECK((ft_isalpha(value) != 0) == (isalpha(value) != 0));
		CHECK((ft_isdigit(value) != 0) == (isdigit(value) != 0));
		CHECK((ft_isalnum(value) != 0) == (isalnum(value) != 0));
		CHECK((ft_isprint(value) != 0) == (isprint(value) != 0));
		CHECK(ft_toupper(value) == toupper(value));
		CHECK(ft_tolower(value) == tolower(value));
		CHECK((ft_isascii(value) != 0) == (value <= 127));
		value++;
	}
	CHECK(ft_toupper(EOF) == EOF);
	CHECK(ft_tolower(EOF) == EOF);
	CHECK(ft_isalpha(-42) == 0);
	CHECK(ft_isascii(-1) == 0);
	CHECK(ft_isascii(128) == 0);
}
