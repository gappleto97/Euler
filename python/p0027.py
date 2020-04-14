"""
Project Euler Problem 27

Another good problem for code golf

Problem:Euler discovered the remarkable quadratic formula:

n**2+n+41

It turns out that the formula will produce 40 primes for the consecutive
integer values 0≤n≤39. However, when n=40,40**2+40+41=40(40+1)+41 is divisible
by 41, and certainly when n=41,41**2+41+41 is clearly divisible by 41.

The incredible formula n**2−79n+1601 was discovered, which produces 80 primes
for the consecutive values 0≤n≤79. The product of the coefficients, −79 and
1601, is −126479.

Considering quadratics of the form:

    n**2+an+b

, where |a|<1000 and |b|≤1000

where |n| is the modulus/absolute value of n, e.g. |11|=11 and |−4|=4

Find the product of the coefficients, a and b, for the quadratic expression
that produces the maximum number of primes for consecutive values of n,
starting with n=0.
"""
import functools
import itertools
from typing import Iterator

import p0003
import p0007


def quadratic(n: int, a: int, b: int) -> int:
    return (n + a) * n + b


def primes_and_negatives(*args) -> Iterator[int]:
    for p in p0003.primes(*args):
        yield p
        yield -p


def main() -> int:
    streak = answer = 0
    for a in range(-999, 1000):
        for b in primes_and_negatives(1001):
            formula = functools.partial(quadratic, a=a, b=b)
            test = sum(1 for _ in itertools.takewhile(p0007.is_prime, map(formula, itertools.count())))
            if test > streak:
                streak = test
                answer = a * b
    return answer


if __name__ == '__main__':
    print(main())
