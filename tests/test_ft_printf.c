#include "ft_printf.h"

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct s_capture
{
	int	saved_stdout;
	int	pipe_fd[2];
}	t_capture;

static void	fail_test(int line, const char *message)
{
	dprintf(STDERR_FILENO, "line %d: %s\n", line, message);
	exit(1);
}

static void	dump_bytes(const char *label, const char *buffer, int length)
{
	int	index;

	dprintf(STDERR_FILENO, "%s:", label);
	index = 0;
	while (index < length)
	{
		dprintf(STDERR_FILENO, " %02x", (unsigned char)buffer[index]);
		index++;
	}
	dprintf(STDERR_FILENO, "\n");
}

static void	capture_begin(t_capture *capture, int line)
{
	fflush(stdout);
	if (pipe(capture->pipe_fd) < 0)
		fail_test(line, "pipe failed");
	capture->saved_stdout = dup(STDOUT_FILENO);
	if (capture->saved_stdout < 0)
		fail_test(line, "dup failed");
	if (dup2(capture->pipe_fd[1], STDOUT_FILENO) < 0)
		fail_test(line, "dup2 failed");
	close(capture->pipe_fd[1]);
}

static ssize_t	capture_end(t_capture *capture, char *buffer,
		size_t capacity, int line)
{
	ssize_t	total;
	ssize_t	read_size;

	fflush(stdout);
	if (dup2(capture->saved_stdout, STDOUT_FILENO) < 0)
		fail_test(line, "stdout restore failed");
	close(capture->saved_stdout);
	total = 0;
	while (1)
	{
		if ((size_t)total == capacity)
			fail_test(line, "capture buffer is full");
		read_size = read(capture->pipe_fd[0], buffer + total,
				capacity - (size_t)total);
		if (read_size < 0)
			fail_test(line, "read failed");
		if (read_size == 0)
			break ;
		total += read_size;
	}
	close(capture->pipe_fd[0]);
	return (total);
}

static void	check_case(int line, const char *format, const char *expected,
		int expected_ret, const char *actual, ssize_t actual_len,
		int actual_ret)
{
	if (actual_ret != expected_ret || actual_len != expected_ret
		|| memcmp(expected, actual, (size_t)expected_ret) != 0)
	{
		dprintf(STDERR_FILENO, "format: %s\n", format);
		dprintf(STDERR_FILENO, "expected ret: %d\n", expected_ret);
		dprintf(STDERR_FILENO, "actual ret: %d, actual bytes: %zd\n",
			actual_ret, actual_len);
		dump_bytes("expected", expected, expected_ret);
		dump_bytes("actual", actual, (int)actual_len);
		fail_test(line, "ft_printf output mismatch");
	}
}

#define EXPECT_PRINTF(FORMAT, ...) do { \
	char		expected[4096]; \
	char		actual[4096]; \
	t_capture	capture; \
	int			expected_ret; \
	int			actual_ret; \
	ssize_t		actual_len; \
	expected_ret = snprintf(expected, sizeof(expected), FORMAT, ##__VA_ARGS__); \
	if (expected_ret < 0 || expected_ret >= (int)sizeof(expected)) \
		fail_test(__LINE__, "snprintf result does not fit test buffer"); \
	capture_begin(&capture, __LINE__); \
	actual_ret = ft_printf(FORMAT, ##__VA_ARGS__); \
	actual_len = capture_end(&capture, actual, sizeof(actual), __LINE__); \
	check_case(__LINE__, FORMAT, expected, expected_ret, actual, actual_len, \
		actual_ret); \
} while (0)

#define EXPECT_OUTPUT(EXPECTED, FORMAT, ...) do { \
	char		actual[4096]; \
	t_capture	capture; \
	int			actual_ret; \
	ssize_t		actual_len; \
	capture_begin(&capture, __LINE__); \
	actual_ret = ft_printf(FORMAT, ##__VA_ARGS__); \
	actual_len = capture_end(&capture, actual, sizeof(actual), __LINE__); \
	check_case(__LINE__, FORMAT, EXPECTED, (int)strlen(EXPECTED), actual, \
		actual_len, actual_ret); \
} while (0)

static void	run_core_cases(void)
{
	EXPECT_PRINTF("plain text");
	EXPECT_PRINTF("percent: %%");
	EXPECT_PRINTF("chars:%c:%c:end", 'A', 0);
	EXPECT_PRINTF("strings:%s", "hello");
	EXPECT_OUTPUT("null-string:(null)", "null-string:%s", (char *)0);
	EXPECT_PRINTF("signed:%d:%i:%d", 0, INT_MIN, INT_MAX);
	EXPECT_PRINTF("unsigned:%u:%u", 0u, UINT_MAX);
	EXPECT_PRINTF("hex:%x:%X:%x", 0u, 48879u, UINT_MAX);
	EXPECT_OUTPUT("ptr:0x0:0x1234", "ptr:%p:%p", (void *)0,
		(void *)(uintptr_t)0x1234);
}

static void	run_bonus_cases(void)
{
	const char	*normalized;

	EXPECT_PRINTF("width:'%8d' '%-8s' '%5c'", 42, "xy", 'z');
	EXPECT_PRINTF("prec:'%.0d' '%.5d' '%.3s'", 0, 42, "abcdef");
	EXPECT_OUTPUT("zero:'-0000042' '0000001f' '0000%'",
		"zero:'%08d' '%08x' '%05%'", -42, 31u);
	EXPECT_PRINTF("flags:'%+d' '% d' '%#x' '%#X'", 7, 7, 48879u, 48879u);
	EXPECT_PRINTF("mix:'%#10.6x' '%-+9.4d'", 42u, 19);
	normalized = "norm:'%-05d' '%+ d'";
	EXPECT_PRINTF(normalized, 12, 12);
}

static void	expect_field_error(int line, const char *format)
{
	char		actual[16];
	t_capture	capture;
	int			actual_ret;
	ssize_t		actual_len;

	capture_begin(&capture, line);
	actual_ret = ft_printf(format, 1);
	actual_len = capture_end(&capture, actual, sizeof(actual), line);
	if (actual_ret != -1 || actual_len != 0)
	{
		dprintf(STDERR_FILENO, "format: %s\n", format);
		dprintf(STDERR_FILENO, "actual ret: %d, actual bytes: %zd\n",
			actual_ret, actual_len);
		dump_bytes("actual", actual, (int)actual_len);
		fail_test(line, "oversized format field was not rejected");
	}
}

static void	run_parser_boundary_cases(void)
{
	expect_field_error(__LINE__, "%2147483648d");
	expect_field_error(__LINE__, "%.2147483648d");
}

int	main(void)
{
	run_core_cases();
	run_bonus_cases();
	run_parser_boundary_cases();
	dprintf(STDERR_FILENO, "ft_printf tests passed\n");
	return (0);
}
