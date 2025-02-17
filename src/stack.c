#include "push_swap.h"

void	stack_init_empty(t_stack *stack)
{
	stack->values = NULL;
	stack->ranks = NULL;
	stack->size = 0;
	stack->capacity = 0;
}

int	stack_init(t_stack *stack, int capacity)
{
	stack_init_empty(stack);
	if (capacity <= 0)
		return (1);
	stack->values = (int *)malloc(sizeof(int) * (size_t)capacity);
	stack->ranks = (int *)malloc(sizeof(int) * (size_t)capacity);
	if (stack->values == NULL || stack->ranks == NULL)
	{
		stack_free(stack);
		return (0);
	}
	stack->capacity = capacity;
	return (1);
}

void	stack_free(t_stack *stack)
{
	free(stack->values);
	free(stack->ranks);
	stack_init_empty(stack);
}

int	stack_is_sorted(const t_stack *stack)
{
	int	i;

	i = 1;
	while (i < stack->size)
	{
		if (stack->ranks[i - 1] > stack->ranks[i])
			return (0);
		i++;
	}
	return (1);
}

int	stack_is_complete_sorted(const t_stack *a, const t_stack *b)
{
	return (b->size == 0 && stack_is_sorted(a));
}
