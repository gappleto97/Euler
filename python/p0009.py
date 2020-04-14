"""
Project Euler Problem 9

My triples() function is definitely pythonic, but I don't know if it is all
that efficient. I feel like I could do better.

Problem:

A Pythagorean triplet is a set of three natural numbers, a < b < c, for which,
a**2 + b**2 = c**2

For example, 3**2 + 4**2 = 9 + 16 = 25 = 5**2.

There exists exactly one Pythagorean triplet for which a + b + c = 1000.
Find the product abc.
"""
import functools
import itertools
import operator
from typing import Iterator, Tuple


def triples() -> Iterator[Tuple[int, int, int]]:
    for c in itertools.count(3):
        for b in range(2, c):
            for a in range(1, b):
                if a**2 + b**2 == c**2:
                    yield (a, b, c)


def main() -> int:
    for triple in triples():
        if sum(triple) == 1000:
            return functools.reduce(operator.mul, triple, 1)
    return -1


if __name__ == '__main__':
    print(main())
