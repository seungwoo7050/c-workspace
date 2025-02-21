#include "push_swap.h"

#include <stdio.h>

typedef struct s_fixture
{
	t_stack	a;
	t_stack	b;
}	t_fixture;

typedef enum e_test_operation
{
	TEST_SA,
	TEST_SB,
	TEST_SS,
	TEST_PA,
	TEST_PB,
	TEST_RA,
	TEST_RB,
	TEST_RR,
	TEST_RRA,
	TEST_RRB,
	TEST_RRR,
	TEST_OPERATION_COUNT
}	t_test_operation;

typedef struct s_expected_fixture
{
	int	a_values[5];
	int	a_ranks[5];
	int	a_size;
	int	b_values[5];
	int	b_ranks[5];
	int	b_size;
}	t_expected_fixture;

static int	expected_rank(int value)
{
	if (value == 40)
		return (4);
	if (value == 10)
		return (1);
	if (value == 30)
		return (3);
	if (value == 20)
		return (2);
	if (value == 0)
		return (0);
	return (-1);
}

static int	stack_pairs_are_valid(const t_stack *stack)
{
	int	i;

	if (stack->size < 0 || stack->size > stack->capacity)
		return (0);
	i = 0;
	while (i < stack->size)
	{
		if (expected_rank(stack->values[i]) != stack->ranks[i])
			return (0);
		i++;
	}
	return (1);
}

static int	all_pairs_are_present(const t_fixture *fixture)
{
	int	seen[5];
	int	i;
	int	rank;

	i = 0;
	while (i < 5)
		seen[i++] = 0;
	i = 0;
	while (i < fixture->a.size)
	{
		rank = fixture->a.ranks[i++];
		if (rank < 0 || rank >= 5 || seen[rank])
			return (0);
		seen[rank] = 1;
	}
	i = 0;
	while (i < fixture->b.size)
	{
		rank = fixture->b.ranks[i++];
		if (rank < 0 || rank >= 5 || seen[rank])
			return (0);
		seen[rank] = 1;
	}
	i = 0;
	while (i < 5)
	{
		if (!seen[i++])
			return (0);
	}
	return (1);
}

static int	fixture_init(t_fixture *fixture)
{
	if (!stack_init(&fixture->a, 5))
		return (0);
	if (!stack_init(&fixture->b, 5))
	{
		stack_free(&fixture->a);
		return (0);
	}
	fixture->a.values[0] = 40;
	fixture->a.ranks[0] = 4;
	fixture->a.values[1] = 10;
	fixture->a.ranks[1] = 1;
	fixture->a.values[2] = 30;
	fixture->a.ranks[2] = 3;
	fixture->a.size = 3;
	fixture->b.values[0] = 20;
	fixture->b.ranks[0] = 2;
	fixture->b.values[1] = 0;
	fixture->b.ranks[1] = 0;
	fixture->b.size = 2;
	return (1);
}

static void	fixture_free(t_fixture *fixture)
{
	stack_free(&fixture->a);
	stack_free(&fixture->b);
}

static const char	*test_operation_name(t_test_operation operation)
{
	static const char *const	names[TEST_OPERATION_COUNT] = {
		"sa", "sb", "ss", "pa", "pb", "ra",
		"rb", "rr", "rra", "rrb", "rrr"
	};

	return (names[operation]);
}

static void	apply_test_operation(t_test_operation operation,
		t_fixture *fixture)
{
	if (operation == TEST_SA)
		op_sa(&fixture->a, 0);
	else if (operation == TEST_SB)
		op_sb(&fixture->b, 0);
	else if (operation == TEST_SS)
		op_ss(&fixture->a, &fixture->b, 0);
	else if (operation == TEST_PA)
		op_pa(&fixture->a, &fixture->b, 0);
	else if (operation == TEST_PB)
		op_pb(&fixture->a, &fixture->b, 0);
	else if (operation == TEST_RA)
		op_ra(&fixture->a, 0);
	else if (operation == TEST_RB)
		op_rb(&fixture->b, 0);
	else if (operation == TEST_RR)
		op_rr(&fixture->a, &fixture->b, 0);
	else if (operation == TEST_RRA)
		op_rra(&fixture->a, 0);
	else if (operation == TEST_RRB)
		op_rrb(&fixture->b, 0);
	else
		op_rrr(&fixture->a, &fixture->b, 0);
}

static int	stack_has_exact_state(const t_stack *stack, const int *values,
		const int *ranks, int size)
{
	int	i;

	if (stack->size != size || stack->capacity != 5)
		return (0);
	i = 0;
	while (i < size)
	{
		if (stack->values[i] != values[i] || stack->ranks[i] != ranks[i])
			return (0);
		i++;
	}
	return (1);
}

static int	fixture_has_exact_state(const t_fixture *fixture,
		const t_expected_fixture *expected)
{
	return (stack_has_exact_state(&fixture->a, expected->a_values,
			expected->a_ranks, expected->a_size)
		&& stack_has_exact_state(&fixture->b, expected->b_values,
			expected->b_ranks, expected->b_size));
}

static int	test_exact_operation_states(void)
{
	static const t_expected_fixture	expected[TEST_OPERATION_COUNT] = {
	{{10, 40, 30}, {1, 4, 3}, 3, {20, 0}, {2, 0}, 2},
	{{40, 10, 30}, {4, 1, 3}, 3, {0, 20}, {0, 2}, 2},
	{{10, 40, 30}, {1, 4, 3}, 3, {0, 20}, {0, 2}, 2},
	{{20, 40, 10, 30}, {2, 4, 1, 3}, 4, {0}, {0}, 1},
	{{10, 30}, {1, 3}, 2, {40, 20, 0}, {4, 2, 0}, 3},
	{{10, 30, 40}, {1, 3, 4}, 3, {20, 0}, {2, 0}, 2},
	{{40, 10, 30}, {4, 1, 3}, 3, {0, 20}, {0, 2}, 2},
	{{10, 30, 40}, {1, 3, 4}, 3, {0, 20}, {0, 2}, 2},
	{{30, 40, 10}, {3, 4, 1}, 3, {20, 0}, {2, 0}, 2},
	{{40, 10, 30}, {4, 1, 3}, 3, {0, 20}, {0, 2}, 2},
	{{30, 40, 10}, {3, 4, 1}, 3, {0, 20}, {0, 2}, 2}
	};
	t_fixture	fixture;
	int			index;

	index = 0;
	while (index < TEST_OPERATION_COUNT)
	{
		if (!fixture_init(&fixture))
			return (0);
		apply_test_operation((t_test_operation)index, &fixture);
		if (!fixture_has_exact_state(&fixture, &expected[index]))
		{
			fprintf(stderr, "%s produced an unexpected stack state\n",
				test_operation_name((t_test_operation)index));
			fixture_free(&fixture);
			return (0);
		}
		fixture_free(&fixture);
		index++;
	}
	return (1);
}

static int	small_fixture_init(t_fixture *fixture, int a_size, int b_size)
{
	if (!stack_init(&fixture->a, 2))
		return (0);
	if (!stack_init(&fixture->b, 2))
		return (stack_free(&fixture->a), 0);
	fixture->a.values[0] = 101;
	fixture->a.values[1] = 102;
	fixture->a.ranks[0] = 11;
	fixture->a.ranks[1] = 12;
	fixture->b.values[0] = 201;
	fixture->b.values[1] = 202;
	fixture->b.ranks[0] = 21;
	fixture->b.ranks[1] = 22;
	fixture->a.size = a_size;
	fixture->b.size = b_size;
	return (1);
}

static int	small_stack_is_unchanged(const t_stack *stack,
		int first_value, int second_value, int first_rank, int second_rank,
		int size)
{
	return (stack->capacity == 2 && stack->size == size
		&& stack->values[0] == first_value
		&& stack->values[1] == second_value
		&& stack->ranks[0] == first_rank
		&& stack->ranks[1] == second_rank);
}

static int	run_noop_case(t_test_operation operation, int a_size, int b_size)
{
	t_fixture	fixture;
	int			valid;

	if (!small_fixture_init(&fixture, a_size, b_size))
		return (0);
	apply_test_operation(operation, &fixture);
	valid = small_stack_is_unchanged(&fixture.a, 101, 102, 11, 12, a_size)
		&& small_stack_is_unchanged(&fixture.b, 201, 202, 21, 22, b_size);
	if (!valid)
		fprintf(stderr, "%s changed an insufficient stack\n",
			test_operation_name(operation));
	fixture_free(&fixture);
	return (valid);
}

static int	test_operation_noops(void)
{
	int	index;

	index = 0;
	while (index < TEST_OPERATION_COUNT)
	{
		if (!run_noop_case((t_test_operation)index, 0, 0))
			return (0);
		if (index == TEST_PA)
		{
			if (!run_noop_case(TEST_PA, 1, 0))
				return (0);
		}
		else if (index == TEST_PB)
		{
			if (!run_noop_case(TEST_PB, 0, 1))
				return (0);
		}
		else if (!run_noop_case((t_test_operation)index, 1, 1))
			return (0);
		index++;
	}
	return (1);
}

static int	fixture_is_valid(const char *operation, const t_fixture *fixture)
{
	if (!stack_pairs_are_valid(&fixture->a)
		|| !stack_pairs_are_valid(&fixture->b)
		|| fixture->a.size + fixture->b.size != 5
		|| !all_pairs_are_present(fixture))
	{
		fprintf(stderr, "%s broke the value/rank pairing invariant\n",
			operation);
		return (0);
	}
	return (1);
}

static int	check_fixture(const char *operation, t_fixture *fixture)
{
	int	valid;

	valid = fixture_is_valid(operation, fixture);
	fixture_free(fixture);
	return (valid);
}

static int	test_operation_sequence(void)
{
	t_fixture	fixture;

	if (!fixture_init(&fixture))
		return (0);
#define APPLY_AND_CHECK(name, call) \
	do { \
		call; \
		if (!fixture_is_valid(name " in sequence", &fixture)) \
			return (fixture_free(&fixture), 0); \
	} while (0)
	APPLY_AND_CHECK("sa", op_sa(&fixture.a, 0));
	APPLY_AND_CHECK("sb", op_sb(&fixture.b, 0));
	APPLY_AND_CHECK("ss", op_ss(&fixture.a, &fixture.b, 0));
	APPLY_AND_CHECK("pa", op_pa(&fixture.a, &fixture.b, 0));
	APPLY_AND_CHECK("pb", op_pb(&fixture.a, &fixture.b, 0));
	APPLY_AND_CHECK("ra", op_ra(&fixture.a, 0));
	APPLY_AND_CHECK("rb", op_rb(&fixture.b, 0));
	APPLY_AND_CHECK("rr", op_rr(&fixture.a, &fixture.b, 0));
	APPLY_AND_CHECK("rra", op_rra(&fixture.a, 0));
	APPLY_AND_CHECK("rrb", op_rrb(&fixture.b, 0));
	APPLY_AND_CHECK("rrr", op_rrr(&fixture.a, &fixture.b, 0));
#undef APPLY_AND_CHECK
	fixture_free(&fixture);
	return (1);
}

static int	test_operations(void)
{
	t_fixture	fixture;

#define RUN_OPERATION(name, call) \
	do { \
		if (!fixture_init(&fixture)) \
			return (0); \
		call; \
		if (!check_fixture(name, &fixture)) \
			return (0); \
	} while (0)
	RUN_OPERATION("sa", op_sa(&fixture.a, 0));
	RUN_OPERATION("sb", op_sb(&fixture.b, 0));
	RUN_OPERATION("ss", op_ss(&fixture.a, &fixture.b, 0));
	RUN_OPERATION("pa", op_pa(&fixture.a, &fixture.b, 0));
	RUN_OPERATION("pb", op_pb(&fixture.a, &fixture.b, 0));
	RUN_OPERATION("ra", op_ra(&fixture.a, 0));
	RUN_OPERATION("rb", op_rb(&fixture.b, 0));
	RUN_OPERATION("rr", op_rr(&fixture.a, &fixture.b, 0));
	RUN_OPERATION("rra", op_rra(&fixture.a, 0));
	RUN_OPERATION("rrb", op_rrb(&fixture.b, 0));
	RUN_OPERATION("rrr", op_rrr(&fixture.a, &fixture.b, 0));
#undef RUN_OPERATION
	return (1);
}

int	main(void)
{
	if (!test_operations() || !test_operation_sequence()
		|| !test_exact_operation_states() || !test_operation_noops())
		return (1);
	printf("operation invariants passed\n");
	return (0);
}
