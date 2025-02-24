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

static void	move_index_to_top(t_stack *a, int index)
{
	if (index <= a->size / 2)
	{
		while (index-- > 0)
			op_ra(a, 1);
	}
	else
	{
		index = a->size - index;
		while (index-- > 0)
			op_rra(a, 1);
	}
}

static void	sort_two(t_stack *a)
{
	if (a->ranks[0] > a->ranks[1])
		op_sa(a, 1);
}

static void	sort_three(t_stack *a)
{
	int	first;
	int	second;
	int	third;

	if (stack_is_sorted(a))
		return ;
	first = a->ranks[0];
	second = a->ranks[1];
	third = a->ranks[2];
	if (first > second && second < third && first < third)
		op_sa(a, 1);
	else if (first > second && second > third)
	{
		op_sa(a, 1);
		op_rra(a, 1);
	}
	else if (first > second && second < third && first > third)
		op_ra(a, 1);
	else if (first < second && second > third && first < third)
	{
		op_sa(a, 1);
		op_ra(a, 1);
	}
	else if (first < second && second > third && first > third)
		op_rra(a, 1);
}

static void	sort_tiny(t_stack *a, t_stack *b)
{
	int	target_rank;
	int	index;

	if (a->size == 2)
	{
		sort_two(a);
		return ;
	}
	target_rank = 0;
	while (a->size > 3)
	{
		index = find_rank_index(a, target_rank);
		move_index_to_top(a, index);
		op_pb(a, b, 1);
		target_rank++;
	}
	sort_three(a);
	while (b->size > 0)
		op_pa(a, b, 1);
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

static void	radix_sort(t_stack *a, t_stack *b)
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
				op_ra(a, 1);
			else
				op_pb(a, b, 1);
			i++;
		}
		while (b->size > 0)
			op_pa(a, b, 1);
		bit++;
	}
}

void	sort_stack(t_stack *a, t_stack *b)
{
	if (a->size < 2 || stack_is_sorted(a))
		return ;
	if (a->size <= 5)
		sort_tiny(a, b);
	else
		radix_sort(a, b);
}
