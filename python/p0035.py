"""
Project Euler Problem 35

This ended up being a filtering problem. The problem with my solution is that I
am not satisfied with my filter at all. I feel like there is a more efficient
way to go about it.

Problem:

The number, 197, is called a circular prime because all rotations of the
digits: 197, 971, and 719, are themselves prime.

There are thirteen such primes below 100: 2, 3, 5, 7, 11, 13, 17, 31, 37, 71,
73, 79, and 97.

How many circular primes are there below one million?
"""
from typing import Iterator

import p0007


def rotations(x: int) -> Iterator[int]:
    r = repr(x)
    for y in range(len(r)):
        yield int(r[y:] + r[:y])


def main() -> int:
    answer = 0
    for x in range(1000000):
        if all(p0007.is_prime(r) for r in rotations(x)):
            answer += 1
    return answer


if __name__ == '__main__':
    print(main())
