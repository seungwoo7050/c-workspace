#include "test.h"

#include "libft.h"

#include <stdlib.h>

void	test_atoi(void)
{
	static const char	*cases[] = {
		"", "+", "-", "0", "+0", "-0", "1", "-1", "42", "-42",
		"2147483647", "-2147483648", "00000123", "-00000123",
		"  17", "\t\n\v\f\r 23", " +98words", "-77tail",
		"words42", "++1", "--1", "+-1", "-+1"
	};
	size_t			index;

	index = 0;
	while (index < sizeof(cases) / sizeof(cases[0]))
	{
		CHECK(ft_atoi(cases[index]) == atoi(cases[index]));
		index++;
	}
	CHECK(ft_atoi("999999999999999999999999") == 2147483647);
	CHECK(ft_atoi("-999999999999999999999999") == (-2147483647 - 1));
}
