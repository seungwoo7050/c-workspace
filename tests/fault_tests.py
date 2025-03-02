#!/usr/bin/env python3
import os
from pathlib import Path
import subprocess
import sys


ROOT = Path(__file__).resolve().parents[1]
PUSH_SWAP = ROOT / os.environ.get("PS_PUSH_SWAP", ".build/fault/push_swap")
CHECKER = ROOT / os.environ.get("PS_CHECKER", ".build/fault/checker")
ALLOCATION_REPORT = b"PS_LIVE_ALLOCATIONS=0\n"


def fail(message):
    print(message, file=sys.stderr)
    raise SystemExit(1)


def assert_equal(actual, expected, message):
    if actual != expected:
        fail(f"{message}: expected {expected!r}, got {actual!r}")


def assert_true(condition, message):
    if not condition:
        fail(message)


def run(binary, args, input_bytes=b"", faults=None):
    environment = os.environ.copy()
    environment["PS_REPORT_ALLOCATIONS"] = "1"
    if faults:
        environment.update({key: str(value) for key, value in faults.items()})
    try:
        result = subprocess.run(
            [str(binary), *[str(arg) for arg in args]],
            input=input_bytes,
            capture_output=True,
            cwd=ROOT,
            env=environment,
            timeout=3,
            check=False,
        )
    except subprocess.TimeoutExpired:
        fail(f"{binary.name} timed out with faults {faults!r}")
    assert_true(
        b"PS_LIVE_ALLOCATIONS=NONZERO" not in result.stderr,
        f"{binary.name} leaked allocations with faults {faults!r}",
    )
    assert_true(
        ALLOCATION_REPORT in result.stderr,
        f"{binary.name} did not report cleanup with faults {faults!r}",
    )
    result.stderr = result.stderr.replace(ALLOCATION_REPORT, b"")
    return result


def test_nth_allocation_failures():
    for index in range(1, 6):
        result = run(
            PUSH_SWAP,
            ["4", "3", "2", "1"],
            faults={"PS_FAIL_MALLOC_AT": index},
        )
        assert_true(result.returncode != 0, f"push_swap malloc #{index} fails")
        assert_equal(result.stderr, b"Error\n", "push_swap allocation error")
    success = run(
        PUSH_SWAP,
        ["4", "3", "2", "1"],
        faults={"PS_FAIL_MALLOC_AT": 6},
    )
    assert_equal(success.returncode, 0, "push_swap past-last malloc succeeds")

    for index in range(1, 7):
        result = run(
            CHECKER,
            ["2", "1"],
            b"sa\n",
            faults={"PS_FAIL_MALLOC_AT": index},
        )
        assert_true(result.returncode != 0, f"checker malloc #{index} fails")
        assert_equal(result.stderr, b"Error\n", "checker allocation error")
    success = run(
        CHECKER,
        ["2", "1"],
        b"sa\n",
        faults={"PS_FAIL_MALLOC_AT": 7},
    )
    assert_equal(success.returncode, 0, "checker past-last malloc succeeds")
    assert_equal(success.stdout, b"OK\n", "checker succeeds after allocation sweep")


def main():
    test_nth_allocation_failures()
    print("fault injection tests passed")


if __name__ == "__main__":
    main()
