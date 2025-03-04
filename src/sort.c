#include "push_swap.h"

static int	find_rank_index(const t_stack *stack, int rank)
{
	int	i;

	i = 0;
	while (i < stack->size)
	{
		if (stack->ranks[i] == rank)
			return (i);
		i++;
	}
	return (-1);
}

static int	move_index_to_top(t_stack *a, int index)
{
	if (index <= a->size / 2)
	{
		while (index-- > 0)
		{
			if (!op_ra(a, 1))
				return (0);
		}
	}
	else
	{
		index = a->size - index;
		while (index-- > 0)
		{
			if (!op_rra(a, 1))
				return (0);
		}
	}
	return (1);
}

static int	sort_two(t_stack *a)
{
	if (a->ranks[0] > a->ranks[1])
		return (op_sa(a, 1));
	return (1);
}

static int	sort_three(t_stack *a)
{
	int	first;
	int	second;
	int	third;

	if (stack_is_sorted(a))
		return (1);
	first = a->ranks[0];
	second = a->ranks[1];
	third = a->ranks[2];
	if (first > second && second < third && first < third)
		return (op_sa(a, 1));
	else if (first > second && second > third)
	{
		if (!op_sa(a, 1))
			return (0);
		return (op_rra(a, 1));
	}
	else if (first > second && second < third && first > third)
		return (op_ra(a, 1));
	else if (first < second && second > third && first < third)
	{
		if (!op_sa(a, 1))
			return (0);
		return (op_ra(a, 1));
	}
	else if (first < second && second > third && first > third)
		return (op_rra(a, 1));
	return (1);
}

static int	sort_tiny(t_stack *a, t_stack *b)
{
	int	target_rank;
	int	index;

	if (a->size == 2)
		return (sort_two(a));
	target_rank = 0;
	while (a->size > 3)
	{
		index = find_rank_index(a, target_rank);
		if (!move_index_to_top(a, index) || !op_pb(a, b, 1))
			return (0);
		target_rank++;
	}
	if (!sort_three(a))
		return (0);
	while (b->size > 0)
	{
		if (!op_pa(a, b, 1))
			return (0);
	}
	return (1);
}

static int	count_bits(int size)
{
	int	bits;
	int	max_rank;

	bits = 0;
	max_rank = size - 1;
	while ((max_rank >> bits) != 0)
		bits++;
	return (bits);
}

static int	radix_sort(t_stack *a, t_stack *b)
{
	int	bits;
	int	bit;
	int	i;
	int	round_size;

	bits = count_bits(a->size);
	bit = 0;
	while (bit < bits)
	{
		round_size = a->size;
		i = 0;
		while (i < round_size)
		{
			if (((a->ranks[0] >> bit) & 1) == 1)
			{
				if (!op_ra(a, 1))
					return (0);
			}
			else if (!op_pb(a, b, 1))
				return (0);
			i++;
		}
		while (b->size > 0)
		{
			if (!op_pa(a, b, 1))
				return (0);
		}
		bit++;
	}
	return (1);
}

int	sort_stack(t_stack *a, t_stack *b)
{
	if (a->size < 2 || stack_is_sorted(a))
		return (1);
	if (a->size <= 5)
		return (sort_tiny(a, b));
	return (radix_sort(a, b));
}
