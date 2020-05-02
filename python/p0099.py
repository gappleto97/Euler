"""
Project Euler Problem 99

Comparing two numbers written in index form like 211 and 37 is not difficult, as any calculator would confirm that 211 = 2048 < 37 = 2187.

However, confirming that 632382518061 > 519432525806 would be much more difficult, as both numbers contain over three million digits.

Using base_exp.txt (right click and 'Save Link/Target As...'), a 22K text file containing one thousand lines with a base/exponent pair on each line, determine which line number has the greatest numerical value.

NOTE: The first two lines in the file represent the numbers in the example given above.
"""
from functools import cmp_to_key
from math import log
from pathlib import Path
from typing import List, Tuple


def cmp_two_exp(pair1: Tuple[int, float], pair2: Tuple[int, float]) -> int:
    base1, exp1 = pair1
    base2, exp2 = pair2
    exp1 *= log(base1)
    exp2 *= log(base2)
    if exp1 == exp2:
        return 0
    elif exp1 > exp2:
        return 1
    return -1


def main() -> int:
    candidates: List[Tuple[int, int]] = []
    with Path(__file__).parent.parent.joinpath('_data', 'p0099_base_exp.txt').open('r') as f:
        for line in f.readlines():
            x, y = line.rstrip('\n').split(',')
            candidates.append((int(x), int(y)))
    return max(enumerate(candidates, 1), key=lambda x: cmp_to_key(cmp_two_exp)(x[1]))[0]


if __name__ == '__main__':
    print(main())
