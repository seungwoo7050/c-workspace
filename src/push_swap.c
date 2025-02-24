#include "push_swap.h"

int	main(int argc, char **argv)
{
	t_stack	a;
	t_stack	b;

	if (!parse_input(argc, argv, &a))
	{
		write_error();
		return (1);
	}
	if (!stack_init(&b, a.capacity))
	{
		stack_free(&a);
		write_error();
		return (1);
	}
	sort_stack(&a, &b);
	stack_free(&a);
	stack_free(&b);
	return (0);
}
