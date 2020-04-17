"""
Project Euler Problem 357

A key note is that n // d is always also a factor, so it ends up being pairs. This means you can halve your search space

Problem:

Consider the divisors of 30: 1,2,3,5,6,10,15,30.
It can be seen that for every divisor d of 30, d+30/d is prime.

Find the sum of all positive integers n not exceeding 100 000 000
such that for every divisor d of n, d+n/d is prime.

Revision 1:

Prime filter cuts by a little more than half, but I think this is mostly from the cache benefits on is_prime().
Prime square filter shaves off an additional ~8%.
without negating its benefit, or figure out a more general solution to the filter.
Filtering to evens also shaves off some more, and n=4k+2 goes even further, about 45%.
This cuts it overall from ~20 minutes to ~5 minutes.
"""
from math import sqrt
import sys
from typing import Iterable

from p0003 import primes, prime_factors
from p0007 import is_prime


def divisors(n: int) -> Iterable[int]:
    yield 1
    divs = {1}
    for fact in prime_factors(n):
        temp = set()
        for div in divs:
            d = fact * div
            if d not in divs:
                yield d
                temp.add(fact * div)
        divs.update(temp)


def main() -> int:
    answer = 1 + 2  # don't bother trying 1, 2, they're correct
    iterator = primes()
    curr_prime = next(iterator)
    prime_squares = {p * p for p in primes(int(sqrt(100000000)) + 1)}
    for n in range(6, 100000000, 4):  # I have no idea why, but it needs to be in the form 4k+2
        while n > curr_prime:
            curr_prime = next(iterator)  # keep the prime cache happy
        for d in divisors(n):
            if d in prime_squares:
                break
                # this works because if n=kp^2, then whenever d=p, (p + kp^2/p) = (k+1)p, which isn't prime
                # but since detecting if n % d^2 == 0 is expensive, I just wait for p^2 to show up
            if not is_prime(d + n // d):
                break
        else:  # read as: if you didn't break
            answer += n
            print(f'{n:,}', answer)
            sys.stdout.flush()
    return answer


if __name__ == '__main__':
    print(main())
