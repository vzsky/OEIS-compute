#!/usr/bin/env python3
import sys
import os
import re

def is_valid_bfile(path):

    if not os.path.isfile(path):
        return False, "file does not exist"

    line_re = re.compile(r"^(-?\d+)\s(-?\d+)$")
    lines = None

    with open(path, "r") as f:
        lines = f.readlines()

    if not lines or lines[-1].strip() != "":
        return False, "file must end with a blank line"

    if not lines or not lines[0].startswith("#"):
        return False, "first line must be the program"

    last_n = None
    for lineno, line in enumerate(lines, 1):
        line = line.strip()
        if not line or line.startswith("#"):
            continue

        m = line_re.match(line)
        if not m:
            return False, f"invalid format at line {lineno}"

        n = int(m.group(1))

        if last_n is not None and n != last_n + 1:
            return False, f"non-consecutive index at line {lineno}"

        last_n = n

    if last_n is None:
        return False, "no data lines found"

    return True, "valid b-file"


def get_first_last(path):
    first_n = None 
    last_n = None
    with open(path, "r") as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue

            n = int(line.split()[0])
            if first_n is None:
                first_n = n
            last_n = n

    return first_n, last_n

def main():
    if len(sys.argv) != 2:
        print(f"usage: {sys.argv[0]} <bfile>")
        sys.exit(1)

    ok, msg = is_valid_bfile(sys.argv[1])
    if ok:
        print("OK:", msg)
        first_n, last_n = get_first_last (sys.argv[1])
        print(f"range: {first_n} .. {last_n}")
        sys.exit(0)
    else:
        print("ERROR:", msg)
        sys.exit(2)

if __name__ == "__main__":
    main()

