"""
Project Euler Problem 77

I was able to recycle a lot of the work on #76 for this, though I did have to undo some optimizations to get there

Problem:

It is possible to write ten as the sum of primes in exactly five different ways:

7 + 3
5 + 5
5 + 3 + 2
3 + 3 + 2 + 2
2 + 2 + 2 + 2 + 2

What is the first value which can be written as the sum of primes in over five thousand different ways?
"""
import itertools
from typing import List, Tuple

import p0003


def prime_summations(n: int) -> int:
    answer = 0
    cached_primes: Tuple[int, ...] = tuple(p0003.primes(n))
    num_primes = len(cached_primes)
    max_idx = num_primes - 1
    counts: List[int] = [0] * num_primes
    # counts is a list containing how many times you add each prime
    # so for 5 + 5 + 3 + 2 it would be [1, 1, 2]
    counts[0] = n // 2  # primes[0] = 2
    while True:
        total = sum(x * y for x, y in zip(counts, cached_primes))
        counts[0] += 1
        if total > n:
            idx = 0
            while total > n and idx < max_idx:
                counts[idx] = 0
                idx += 1
                counts[idx] += 1
                total = sum(x * y for x, y in zip(counts, cached_primes))
            if idx >= max_idx:
                break
            counts[0] = (n - total) // 2  # primes[0] = 2
        elif total == n:
            answer += 1
    return answer


def main() -> int:
    for x in itertools.count(11):
        if prime_summations(x) > 5_000:
            return x
    return -1


if __name__ == '__main__':
    print(main())
