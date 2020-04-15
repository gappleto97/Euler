"""
Project Euler Problem 57

Problem:

It is possible to show that the square root of two can be expressed as an infinite continued fraction.

√ 2 = 1 + 1/(2 + 1/(2 + 1/(2 + ... ))) = 1.414213...

By expanding this for the first four iterations, we get:

1 + 1/2 = 3/2 = 1.5
1 + 1/(2 + 1/2) = 7/5 = 1.4
1 + 1/(2 + 1/(2 + 1/2)) = 17/12 = 1.41666...
1 + 1/(2 + 1/(2 + 1/(2 + 1/2))) = 41/29 = 1.41379...

The next three expansions are 99/70, 239/169, and 577/408, but the eighth expansion, 1393/985, is the first example
where the number of digits in the numerator exceeds the number of digits in the denominator.

In the first one-thousand expansions, how many fractions contain a numerator with more digits than denominator?
"""
from fractions import Fraction
from functools import lru_cache

import p0052


@lru_cache()
def root_two_denominator(n: int) -> Fraction:
    if n == 0:
        return Fraction(2, 1)
    tmp = root_two_denominator(n - 1)
    return Fraction(1 + 2 * tmp, tmp)


def root_two_expansion(n: int) -> Fraction:
    tmp = root_two_denominator(n)
    return Fraction(1 + tmp, tmp)


def main() -> int:
    answer = 0
    for x in range(1_000):
        frac = root_two_expansion(x)
        if len(p0052.digits(frac.numerator)) > len(p0052.digits(frac.denominator)):
            answer += 1
    return answer


if __name__ == '__main__':
    print(main())
