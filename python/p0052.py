"""
Project Euler Problem 52

Problem:

It can be seen that the number, 125874, and its double, 251748, contain exactly the same digits, but in a different
order.

Find the smallest positive integer, x, such that 2x, 3x, 4x, 5x, and 6x, contain the same digits.

Revision 1:

Changed to return a list instead of a set so it can be used in other problems
"""
import itertools
import math
from typing import List


def digits(n: int) -> List[int]:
    ret = [0] * (int(math.log10(n)) + 1)
    idx = -1
    while n:
        n, digit = divmod(n, 10)
        ret[idx] = digit
        idx -= 1
    return ret


def main():
    for x in itertools.count(1):
        orig = set(digits(x))
        if all(set(digits(x * y)) == orig for y in range(2, 7)):
            return x


if __name__ == '__main__':
    print(main())
