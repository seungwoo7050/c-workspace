#include "push_swap.h"

static int	is_space(char c)
{
	return (c == ' ' || c == '\t' || c == '\n'
		|| c == '\r' || c == '\v' || c == '\f');
}

static size_t	count_tokens_in_arg(const char *arg)
{
	size_t	count;
	size_t	i;

	count = 0;
	i = 0;
	while (arg[i] != '\0')
	{
		while (is_space(arg[i]))
			i++;
		if (arg[i] == '\0')
			break ;
		if (count == (size_t)INT_MAX)
			return ((size_t)INT_MAX + 1);
		count++;
		while (arg[i] != '\0' && !is_space(arg[i]))
			i++;
	}
	return (count);
}

static int	count_all_tokens(int argc, char **argv, int *result)
{
	size_t	count;
	int	i;
	size_t	argument_count;

	count = 0;
	i = 1;
	while (i < argc)
	{
		argument_count = count_tokens_in_arg(argv[i]);
		if (argument_count > (size_t)INT_MAX - count)
			return (0);
		count += argument_count;
		i++;
	}
	*result = (int)count;
	return (1);
}

static int	parse_token(const char *arg, size_t start, size_t end, int *out)
{
	long long	value;
	long long	limit;
	int			sign;
	size_t		i;

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
	size_t	start;
	size_t	end;
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

static int	compare_ints(const void *left, const void *right)
{
	int	a;
	int	b;

	a = *(const int *)left;
	b = *(const int *)right;
	return ((a > b) - (a < b));
}

static int	find_rank(const int *sorted, int size, int value)
{
	int	low;
	int	high;
	int	mid;

	low = 0;
	high = size;
	while (low < high)
	{
		mid = low + (high - low) / 2;
		if (sorted[mid] < value)
			low = mid + 1;
		else
			high = mid;
	}
	return (low);
}

static int	assign_ranks(t_stack *a)
{
	int	*sorted;
	int	i;

	sorted = (int *)ps_malloc(sizeof(int) * (size_t)a->size);
	if (sorted == NULL)
		return (0);
	i = 0;
	while (i < a->size)
	{
		sorted[i] = a->values[i];
		i++;
	}
	qsort(sorted, (size_t)a->size, sizeof(int), compare_ints);
	i = 1;
	while (i < a->size)
	{
		if (sorted[i - 1] == sorted[i])
		{
			ps_free(sorted);
			return (0);
		}
		i++;
	}
	i = 0;
	while (i < a->size)
	{
		a->ranks[i] = find_rank(sorted, a->size, a->values[i]);
		i++;
	}
	ps_free(sorted);
	return (1);
}

int	parse_input(int argc, char **argv, t_stack *a)
{
	int	count;

	stack_init_empty(a);
	if (argc == 1)
		return (1);
	if (!count_all_tokens(argc, argv, &count))
		return (0);
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
	if (!assign_ranks(a))
	{
		stack_free(a);
		return (0);
	}
	return (1);
}
