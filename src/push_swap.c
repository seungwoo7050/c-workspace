#include "push_swap.h"

int	main(int argc, char **argv)
{
	t_stack	a;
	t_stack	b;
	int		status;

	if (!ps_ignore_sigpipe())
		return (write_error(), ps_test_finish(1));
	if (!parse_input(argc, argv, &a))
		return (write_error(), ps_test_finish(1));
	if (!stack_init(&b, a.capacity))
	{
		stack_free(&a);
		return (write_error(), ps_test_finish(1));
	}
	status = 0;
	if (!sort_stack(&a, &b))
		status = 1;
	stack_free(&a);
	stack_free(&b);
	if (status != 0)
		write_error();
	return (ps_test_finish(status));
}
