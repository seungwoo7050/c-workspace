#include "test.h"

#include "libft.h"

#include <limits.h>
#include <stdlib.h>
#include <string.h>

static void	check_itoa(int number, const char *expected)
{
	char	*actual;

	actual = ft_itoa(number);
	CHECK(actual != NULL);
	if (actual != NULL)
	{
		CHECK(strcmp(actual, expected) == 0);
		free(actual);
	}
}

void	test_itoa(void)
{
	check_itoa(INT_MIN, "-2147483648");
	check_itoa(-100, "-100");
	check_itoa(-10, "-10");
	check_itoa(-9, "-9");
	check_itoa(-1, "-1");
	check_itoa(0, "0");
	check_itoa(1, "1");
	check_itoa(9, "9");
	check_itoa(10, "10");
	check_itoa(99, "99");
	check_itoa(100, "100");
	check_itoa(INT_MAX, "2147483647");
}
