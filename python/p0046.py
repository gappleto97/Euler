"""
Project Euler Problem 46

Didn't even need to rearrange the formula to solve it trivially

Revision 1:

It turns out that rearranging the formula let me cut the time by ~50x

Problem:

It was proposed by Christian Goldbach that every odd composite number can be written as the sum of a prime and twice a
square.

9 = 7 + 2×1^2
15 = 7 + 2×2^2
21 = 3 + 2×3^2
25 = 7 + 2×3^2
27 = 19 + 2×2^2
33 = 31 + 2×1^2

It turns out that the conjecture was false.

What is the smallest odd composite that cannot be written as the sum of a prime and twice a square?
"""
import itertools
import math

import p0003
import p0007


def main():
    cached_primes = tuple(p0003.primes(6000))
    for goal in itertools.count(35, 2):
        if p0007.is_prime(goal):
            continue
        for p in itertools.takewhile(goal.__gt__, cached_primes):
            done = False
            for x in range(1, math.ceil(math.sqrt((goal - p) / 2)) + 1):
                if p + 2 * x * x == goal:
                    done = True
                    break
            if done:
                break
        else:
            return goal


if __name__ == '__main__':
    print(main())
