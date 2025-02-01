#include "ft_printf.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef enum e_write_action
{
	WRITE_ALL,
	WRITE_PART,
	WRITE_EINTR,
	WRITE_EPIPE,
	WRITE_ZERO
}	t_write_action;

typedef struct s_write_step
{
	t_write_action	action;
	size_t			amount;
}	t_write_step;

static t_write_step	g_steps[16];
static int			g_step_count;
static int			g_step_index;
static int			g_write_calls;
static size_t		g_largest_write;
static char			g_output[4096];
static size_t		g_output_length;

static void	fail_test(int line, const char *message)
{
	dprintf(STDERR_FILENO, "line %d: %s\n", line, message);
	exit(1);
}

static void	reset_writer(void)
{
	g_step_count = 0;
	g_step_index = 0;
	g_write_calls = 0;
	g_largest_write = 0;
	g_output_length = 0;
}

static void	add_step(t_write_action action, size_t amount)
{
	if (g_step_count >= (int)(sizeof(g_steps) / sizeof(g_steps[0])))
		fail_test(__LINE__, "too many write steps");
	g_steps[g_step_count].action = action;
	g_steps[g_step_count].amount = amount;
	g_step_count++;
}

static ssize_t	record_bytes(const void *buffer, size_t length)
{
	if (g_output_length + length > sizeof(g_output))
		fail_test(__LINE__, "mock output buffer is full");
	memcpy(g_output + g_output_length, buffer, length);
	g_output_length += length;
	return ((ssize_t)length);
}

ssize_t	ft_printf_test_write(int fd, const void *buffer, size_t length)
{
	t_write_step	step;
	size_t			amount;

	(void)fd;
	g_write_calls++;
	if (length > g_largest_write)
		g_largest_write = length;
	step.action = WRITE_ALL;
	step.amount = 0;
	if (g_step_index < g_step_count)
		step = g_steps[g_step_index++];
	if (step.action == WRITE_EINTR)
	{
		errno = EINTR;
		return (-1);
	}
	if (step.action == WRITE_EPIPE)
	{
		errno = EPIPE;
		return (-1);
	}
	if (step.action == WRITE_ZERO)
		return (0);
	if (step.action == WRITE_PART)
	{
		amount = step.amount;
		if (amount > length)
			amount = length;
		return (record_bytes(buffer, amount));
	}
	return (record_bytes(buffer, length));
}

static void	expect_success(const char *expected, int expected_calls,
		size_t expected_largest_write)
{
	int	result;

	result = ft_printf("%s", expected);
	if (result != (int)strlen(expected))
		fail_test(__LINE__, "successful output returned the wrong length");
	if (g_output_length != strlen(expected)
		|| memcmp(g_output, expected, g_output_length) != 0)
		fail_test(__LINE__, "successful output bytes do not match");
	if (g_write_calls != expected_calls)
		fail_test(__LINE__, "unexpected write call count");
	if (g_largest_write != expected_largest_write)
		fail_test(__LINE__, "unexpected largest write size");
}

static void	run_retry_cases(void)
{
	reset_writer();
	add_step(WRITE_PART, 2);
	add_step(WRITE_ALL, 0);
	expect_success("partial", 2, 7);
	reset_writer();
	add_step(WRITE_EINTR, 0);
	add_step(WRITE_PART, 3);
	add_step(WRITE_EINTR, 0);
	add_step(WRITE_ALL, 0);
	expect_success("interrupt", 4, 9);
}

static void	run_failure_cases(void)
{
	int	result;

	reset_writer();
	add_step(WRITE_EPIPE, 0);
	result = ft_printf("broken");
	if (result != -1 || g_output_length != 0)
		fail_test(__LINE__, "EPIPE was not reported");
	reset_writer();
	add_step(WRITE_PART, 3);
	add_step(WRITE_EPIPE, 0);
	result = ft_printf("%s", "partial failure");
	if (result != -1 || g_output_length != 3
		|| memcmp(g_output, "par", 3) != 0)
		fail_test(__LINE__, "failure after a partial write was not preserved");
	reset_writer();
	add_step(WRITE_ZERO, 0);
	result = ft_printf("zero");
	if (result != -1 || g_output_length != 0)
		fail_test(__LINE__, "zero-byte write was not reported");
}

static void	run_padding_case(void)
{
	int	result;

	reset_writer();
	result = ft_printf("%1000d", 7);
	if (result != 1000 || g_output_length != 1000)
		fail_test(__LINE__, "padded output length mismatch");
	if (g_write_calls != 17)
		fail_test(__LINE__, "padding was not emitted in bounded chunks");
	if (g_largest_write != 64)
		fail_test(__LINE__, "padding chunk size changed");
	if (g_output[998] != ' ' || g_output[999] != '7')
		fail_test(__LINE__, "padded output bytes do not match");
}

int	main(void)
{
	run_retry_cases();
	run_failure_cases();
	run_padding_case();
	dprintf(STDERR_FILENO, "ft_printf output fault tests passed\n");
	return (0);
}
