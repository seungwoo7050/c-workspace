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

void	stack_push(t_stack *dst, t_stack *src)
{
	if (src->size == 0)
		return ;
	if (dst->size > 0)
	{
		memmove(dst->values + 1, dst->values,
			sizeof(int) * (size_t)dst->size);
		memmove(dst->ranks + 1, dst->ranks,
			sizeof(int) * (size_t)dst->size);
	}
	dst->values[0] = src->values[0];
	dst->ranks[0] = src->ranks[0];
	dst->size++;
	src->size--;
	if (src->size > 0)
	{
		memmove(src->values, src->values + 1,
			sizeof(int) * (size_t)src->size);
		memmove(src->ranks, src->ranks + 1,
			sizeof(int) * (size_t)src->size);
	}
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

void	op_pa(t_stack *a, t_stack *b, int emit)
{
	stack_push(a, b);
	emit_op("pa\n", emit);
}

void	op_pb(t_stack *a, t_stack *b, int emit)
{
	stack_push(b, a);
	emit_op("pb\n", emit);
}
