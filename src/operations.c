#include "push_swap.h"

#include <string.h>

static void	emit_op(const char *name, int emit)
{
	if (emit)
		ps_putstr_fd(1, name);
}

void	stack_swap(t_stack *stack)
{
	int	value;
	int	rank;

	if (stack->size < 2)
		return ;
	value = stack->values[0];
	rank = stack->ranks[0];
	stack->values[0] = stack->values[1];
	stack->ranks[0] = stack->ranks[1];
	stack->values[1] = value;
	stack->ranks[1] = rank;
}

void	op_sa(t_stack *a, int emit)
{
	stack_swap(a);
	emit_op("sa\n", emit);
}

void	op_sb(t_stack *b, int emit)
{
	stack_swap(b);
	emit_op("sb\n", emit);
}

void	op_ss(t_stack *a, t_stack *b, int emit)
{
	stack_swap(a);
	stack_swap(b);
	emit_op("ss\n", emit);
}
