#!/usr/bin/env python3
import sys
import os
import re

def read_file (path):
    if not os.path.isfile(path):
        raise Exception("file does not exist")

    lines = None
    result = []

    with open(path, "r") as f:
        lines = f.readlines()

    line_re = re.compile(r"^(-?\d+)\s(-?\d+)$")

    last_n = None
    for lineno, line in enumerate(lines, 1):
        line = line.strip()
        if not line or line.startswith("#"):
            continue

        m = line_re.match(line)
        if not m:
            raise Exception (f"invalid format at line {lineno}")

        n = int(m.group(1))

        if last_n is not None and n != last_n + 1:
            raise Exception (f"non-consecutive index at line {lineno}")

        last_n = n
        result.append(m.group(2))

    if last_n is None:
        raise Exception ("no data lines found")

    return result


def is_valid_bfile(path):
    try:
        _ = read_file(path)

        lines = None
        with open(path, "r") as f:
            lines = f.readlines()

        if not lines or lines[-1].strip() != "":
            raise Exception("file must end with a blank line")

        # if not lines or not lines[0].startswith("#"):
        #     raise Exception("first line must be the program")

    except Exception as e: 
        return False, str(e);
    return True, "valid"


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

def is_compatible (path, ref_path):
    try:
        lines = read_file(path)
        lines_ref = read_file(ref_path)
    except Exception as e:
        print(f"invalid file: {e}")
        return False

    for i, [l, r] in enumerate(zip(lines, lines_ref)): 
        if l != r: 
            print("====== mismatch ======")
            print("index: ", i)
            print("file:", l)
            print("ref:", r)
            return False

    return True 


def main():
    if len(sys.argv) != 2 and len(sys.argv) != 3:
        print(f"usage1: {sys.argv[0]} <bfile>")
        print(f"usage2: {sys.argv[0]} <bfile> <bfile_reference>")
        sys.exit(1)

    ok, msg = is_valid_bfile(sys.argv[1])
    if ok:
        print("OK:", msg)
        first_n, last_n = get_first_last (sys.argv[1])
        print(f"range: {first_n} .. {last_n}")
    else:
        print("ERROR:", msg)
        sys.exit(2)

    if len(sys.argv) == 3: 
        if is_compatible (sys.argv[1], sys.argv[2]) :
            print("Checked: against reference")
        else : 
            print("Error: different sequence in bfiles")
            sys.exit(2)

if __name__ == "__main__":
    main()

