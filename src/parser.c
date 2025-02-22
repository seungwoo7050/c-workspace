#include "push_swap.h"

static int	is_space(char c)
{
	return (c == ' ' || c == '\t' || c == '\n'
		|| c == '\r' || c == '\v' || c == '\f');
}

static int	count_tokens_in_arg(const char *arg)
{
	int	count;
	int	i;

	count = 0;
	i = 0;
	while (arg[i] != '\0')
	{
		while (is_space(arg[i]))
			i++;
		if (arg[i] == '\0')
			break ;
		count++;
		while (arg[i] != '\0' && !is_space(arg[i]))
			i++;
	}
	return (count);
}

static int	count_all_tokens(int argc, char **argv)
{
	int	count;
	int	i;

	count = 0;
	i = 1;
	while (i < argc)
	{
		count += count_tokens_in_arg(argv[i]);
		i++;
	}
	return (count);
}

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

static int	fill_values(int argc, char **argv, t_stack *a)
{
	int	i;
	int	pos;
	int	start;
	int	end;
	int	value;

	i = 1;
	pos = 0;
	while (i < argc)
	{
		end = 0;
		while (argv[i][end] != '\0')
		{
			while (is_space(argv[i][end]))
				end++;
			if (argv[i][end] == '\0')
				break ;
			start = end;
			while (argv[i][end] != '\0' && !is_space(argv[i][end]))
				end++;
			if (!parse_token(argv[i], start, end, &value))
				return (0);
			a->values[pos] = value;
			a->ranks[pos] = value;
			pos++;
		}
		i++;
	}
	return (1);
}

int	parse_input(int argc, char **argv, t_stack *a)
{
	int	count;

	stack_init_empty(a);
	if (argc == 1)
		return (1);
	count = count_all_tokens(argc, argv);
	if (count == 0)
		return (0);
	if (!stack_init(a, count))
		return (0);
	if (!fill_values(argc, argv, a))
	{
		stack_free(a);
		return (0);
	}
	a->size = count;
	return (1);
}
