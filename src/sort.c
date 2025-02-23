#include "push_swap.h"

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

void	sort_stack(t_stack *a, t_stack *b)
{
	(void)b;
	if (a->size < 2 || stack_is_sorted(a))
		return ;
	if (a->size == 2)
		sort_two(a);
	else if (a->size == 3)
		sort_three(a);
}
