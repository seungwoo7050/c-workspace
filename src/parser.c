#include "push_swap.h"

static int	parse_token(const char *arg, int start, int end, int *out)
{
	long long	value;
	long long	limit;
	int			sign;
	int			i;

	value = 0;
	sign = 1;
	i = start;
	if (arg[i] == '+' || arg[i] == '-')
	{
		if (arg[i] == '-')
			sign = -1;
		i++;
	}
	if (i == end)
		return (0);
	limit = INT_MAX;
	if (sign < 0)
		limit = (long long)INT_MAX + 1;
	while (i < end)
	{
		if (arg[i] < '0' || arg[i] > '9')
			return (0);
		value = value * 10 + (arg[i] - '0');
		if (value > limit)
			return (0);
		i++;
	}
	*out = (int)(value * sign);
	return (1);
}

int	parse_input(int argc, char **argv, t_stack *a)
{
	int	count;
	int	index;
	int	value;

	stack_init_empty(a);
	if (argc == 1)
		return (1);
	count = argc - 1;
	if (!stack_init(a, count))
		return (0);
	index = 0;
	while (index < count)
	{
		if (!parse_token(argv[index + 1], 0,
				(int)ps_strlen(argv[index + 1]), &value))
		{
			stack_free(a);
			return (0);
		}
		a->values[index] = value;
		a->ranks[index] = value;
		index++;
	}
	a->size = count;
	return (1);
}
