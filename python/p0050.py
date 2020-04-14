"""
Project Euler Problem 50

Again, surprised how effective the brute force solution was

Revision 1:

Old solution stopped working for some reason. Re-did it starting from the biggest possible space this time

Problem:

The prime 41, can be written as the sum of six consecutive primes:
41 = 2 + 3 + 5 + 7 + 11 + 13

This is the longest sum of consecutive primes that adds to a prime below one-hundred.

The longest sum of consecutive primes below one-thousand that adds to a prime, contains 21 terms, and is equal to 953.

Which prime, below one-million, can be written as the sum of the most consecutive primes?
"""
from typing import List

import p0003
import p0007
import p0008


def main() -> int:
    iter_primes = iter(p0003.primes())
    cached_primes: List[int] = []
    while sum(cached_primes) < 1_000_000:
        cached_primes.append(next(iter_primes))
    cached_primes.pop()
    for number in range(len(cached_primes), 21, -1):
        for group in p0008.groupwise(cached_primes, number):
            total = sum(group)
            if p0007.is_prime(total):
                return total
    return -1


if __name__ == '__main__':
    print(main())
