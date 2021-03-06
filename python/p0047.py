"""
Project Euler Problem 47

I was able to chain this with a previous problem. Probably a suboptimal
solution because of it, but it felt prettier this way.

I was able to add a short-circuited fail case to the is_prime() method, though

Problem:

The first two consecutive numbers to have two distinct prime factors are:

14 = 2 × 7
15 = 3 × 5

The first three consecutive numbers to have three distinct prime factors are:

644 = 2² × 7 × 23
645 = 3 × 5 × 43
646 = 2 × 17 × 19.

Find the first four consecutive integers to have four distinct prime factors
each. What is the first of these numbers?
"""
from functools import lru_cache
from itertools import count
from typing import Tuple

from p0007 import is_prime
from p0008 import groupwise


@lru_cache()
def cached_is_prime(args: Tuple[int, int, bool]) -> bool:
    return is_prime(*args)


def main() -> int:
    for group in groupwise(count(2), 4):
        # print(group[0])
        if all(cached_is_prime((x, 4, True)) for x in group):
            return group[0]
    return -1


if __name__ == '__main__':
    print(main())
