#!/usr/bin/env python3
from pathlib import Path
import subprocess
import sys
import tempfile


ROOT = Path(__file__).resolve().parents[1]
PUSH_SWAP = ROOT / "push_swap"
CHECKER = ROOT / "checker"
CHILD_TIMEOUT_SECONDS = 5


def run(args, input_text=None):
    try:
        return subprocess.run(
            [str(arg) for arg in args],
            input=input_text,
            text=True,
            capture_output=True,
            cwd=ROOT,
            timeout=CHILD_TIMEOUT_SECONDS,
            check=False,
        )
    except subprocess.TimeoutExpired:
        fail(f"child process exceeded {CHILD_TIMEOUT_SECONDS} seconds: {args!r}")


def fail(message):
    print(message, file=sys.stderr)
    raise SystemExit(1)


def assert_equal(actual, expected, message):
    if actual != expected:
        fail(f"{message}: expected {expected!r}, got {actual!r}")


def assert_ok(condition, message):
    if not condition:
        fail(message)


def test_parser_inputs():
    no_args = run([PUSH_SWAP])
    assert_equal(no_args.returncode, 0, "push_swap without args exits cleanly")
    assert_equal(no_args.stdout, "", "push_swap without args has no stdout")
    assert_equal(no_args.stderr, "", "push_swap without args has no stderr")

    valid = run([PUSH_SWAP, "3 2", "1"])
    assert_equal(valid.returncode, 0, "quoted and split argv are accepted")
    checked = run([CHECKER, "3 2", "1"], valid.stdout)
    assert_equal(checked.returncode, 0, "checker accepts generated moves")
    assert_equal(checked.stdout, "OK\n", "generated moves sort quoted input")

    invalid_cases = [
        ["1", "2", "2"],
        ["2147483648"],
        ["-2147483649"],
        ["12a"],
        ["+"],
        [""],
        ["1", "2 1"],
    ]
    for case in invalid_cases:
        result = run([PUSH_SWAP] + case)
        assert_ok(result.returncode != 0, f"invalid input {case!r} fails")
        assert_equal(result.stdout, "", f"invalid input {case!r} has no stdout")
        assert_equal(result.stderr, "Error\n", f"invalid input {case!r} reports Error")


def assert_parser_accepts(args, label):
    result = run([PUSH_SWAP] + args)
    assert_equal(result.returncode, 0, f"{label} is accepted")
    assert_equal(result.stderr, "", f"{label} has no stderr")
    checked = run([CHECKER] + args, result.stdout)
    assert_equal(checked.returncode, 0, f"checker accepts {label}")
    assert_equal(checked.stdout, "OK\n", f"{label} sorts correctly")
    assert_equal(checked.stderr, "", f"checker {label} has no stderr")


def test_parser_boundaries():
    valid_cases = [
        (["+7"], "explicit plus sign"),
        (["-0"], "negative zero"),
        (["0003", "0002", "0001"], "leading zeroes"),
        (["3", "", "2 1"], "empty argv mixed with values"),
        (["3 2\t1\n0\r-1\v-2\f-3"], "all C whitespace separators"),
        (["-2147483648", "0", "2147483647"], "exact int bounds"),
    ]
    for args, label in valid_cases:
        assert_parser_accepts(args, label)

    invalid_cases = [
        ([" \t\n\r\v\f"], "whitespace-only argv"),
        (["9" * 4096], "overlong integer"),
        (["\u0661"], "non-ASCII digit"),
        (["++1"], "repeated plus sign"),
        (["--1"], "repeated minus sign"),
        (["+-1"], "mixed signs"),
        (["-0", "+0"], "signed duplicate zero"),
    ]
    for args, label in invalid_cases:
        result = run([PUSH_SWAP] + args)
        assert_equal(result.returncode, 1, f"{label} fails")
        assert_equal(result.stdout, "", f"{label} has no stdout")
        assert_equal(result.stderr, "Error\n", f"{label} reports Error")


def test_checker_without_values_does_not_read_stdin():
    with tempfile.TemporaryFile() as input_file:
        input_file.write(b"sa\n")
        input_file.flush()
        input_file.seek(0)
        try:
            result = subprocess.run(
                [str(CHECKER)],
                stdin=input_file,
                text=True,
                capture_output=True,
                cwd=ROOT,
                timeout=CHILD_TIMEOUT_SECONDS,
                check=False,
            )
        except subprocess.TimeoutExpired:
            fail("checker without values timed out")
        assert_equal(result.returncode, 0, "checker without values exits cleanly")
        assert_equal(result.stdout, "", "checker without values has no stdout")
        assert_equal(result.stderr, "", "checker without values has no stderr")
        assert_equal(input_file.tell(), 0, "checker without values leaves stdin unread")


def checker_ok(args, program, label):
    result = run([CHECKER] + args, program)
    assert_equal(result.returncode, 0, f"{label} checker exit")
    assert_equal(result.stdout, "OK\n", f"{label} checker stdout")
    assert_equal(result.stderr, "", f"{label} checker stderr")


def test_checker_operations():
    cases = [
        ("sa", ["2", "1"], "sa\n"),
        ("sb", ["2", "1", "3"], "pb\npb\nsb\npa\npa\n"),
        ("ss", ["2", "1", "4", "3"], "pb\npb\nss\npa\npa\n"),
        ("pa-pb", ["1", "2"], "pb\npa\n"),
        ("ra", ["3", "1", "2"], "ra\n"),
        ("rb", ["2", "1", "3"], "pb\npb\nrb\npa\npa\n"),
        ("rr", ["2", "1", "4", "3"], "pb\npb\nrr\npa\npa\n"),
        ("rra", ["2", "3", "1"], "rra\n"),
        ("rrb", ["2", "1", "3"], "pb\npb\nrrb\npa\npa\n"),
        ("rrr", ["2", "1", "4", "3"], "pb\npb\nrrr\npa\npa\n"),
    ]
    for label, args, program in cases:
        checker_ok(args, program, label)

    ko = run([CHECKER, "2", "1"], "")
    assert_equal(ko.returncode, 0, "unsorted checker input exits cleanly")
    assert_equal(ko.stdout, "KO\n", "unsorted checker input reports KO")
    assert_equal(ko.stderr, "", "unsorted checker input has no stderr")

    invalid = run([CHECKER, "1", "2"], "ra\nwat\n")
    assert_ok(invalid.returncode != 0, "invalid checker command fails")
    assert_equal(invalid.stdout, "", "invalid checker command has no stdout")
    assert_equal(invalid.stderr, "Error\n", "invalid checker command reports Error")


def main():
    test_parser_inputs()
    test_parser_boundaries()
    test_checker_without_values_does_not_read_stdin()
    test_checker_operations()
    print("tests passed")


if __name__ == "__main__":
    main()
