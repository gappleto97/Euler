"""
Project Euler Problem 23

I had to approach this by modifying the factors function from p0003, but it
seemed to work fairly well.

Problem:

A perfect number is a number for which the sum of its proper divisors is
exactly equal to the number. For example, the sum of the proper divisors of 28
would be 1 + 2 + 4 + 7 + 14 = 28, which means that 28 is a perfect number.

A number n is called deficient if the sum of its proper divisors is less than n
and it is called abundant if this sum exceeds n.

As 12 is the smallest abundant number, 1 + 2 + 3 + 4 + 6 = 16, the smallest
number that can be written as the sum of two abundant numbers is 24. By
mathematical analysis, it can be shown that all integers greater than 28123 can
be written as the sum of two abundant numbers. However, this upper limit cannot
be reduced any further by analysis even though it is known that the greatest
number that cannot be expressed as the sum of two abundant numbers is less than
this limit.

Find the sum of all the positive integers which cannot be written as the sum of
two abundant numbers.
"""
import itertools
from typing import Iterator

import p0021


def abundants() -> Iterator[int]:
    for x in range(12, 28112):
        if sum(p0021.proper_divisors(x)) > x:
            yield x


def main() -> int:
    abundant_sums = set((24, ))
    for x, y in itertools.combinations_with_replacement(abundants(), 2):
        abundant_sums.add(x + y)
    return sum(itertools.filterfalse(abundant_sums.__contains__, range(1, 28124)))


if __name__ == '__main__':
    print(main())
