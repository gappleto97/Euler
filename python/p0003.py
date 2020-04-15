"""
Project Euler Problem 3

More lazy functions this time. Took a little while to figure out how to
eliminate the special case for 2.

Revision 1:

I changed the implementation of prime_factors() to stop checking if factors
exceed the square root of the number. This eliminates a lot of checking for
numbers which could not possibly be the factor.

Revision 2:

Changed prime_factors to catch accidental 0s and -1s, further optimize math.sqrt
checks, and added a persistant cache of primes to reduce the space that needs
checking

Revision 3:

Move primes() to p0003 in order to fix caching. Switch to a prime number sieve
for generating new primes for the cache.

Problem:

The prime factors of 13195 are 5, 7, 13 and 29.

What is the largest prime factor of the number 600851475143 ?
"""
import itertools
import math
import pathlib
from typing import Dict, Iterator, Optional

import sortedcontainers
import umsgpack

cache_filename = 'p0003_cache.mpack'

try:
    with pathlib.Path(__file__).parent.joinpath(cache_filename).open('rb') as f:
        cache = sortedcontainers.SortedSet(umsgpack.load(f))
except Exception:
    cache = sortedcontainers.SortedSet([
        2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61
    ])
last_cached: int = cache[-1] + 2


def primes(stop: Optional[int] = None) -> Iterator[int]:
    if stop is None:
        yield from cache
    else:
        yield from itertools.takewhile(stop.__gt__, cache)
    global last_cached
    if stop is None:
        secondary = modified_eratosthenes()
    elif last_cached - 2 > stop:
        return
    else:
        secondary = itertools.takewhile(stop.__gt__, modified_eratosthenes())
    for p in secondary:
        if p in cache:
            continue
        cache.add(p)
        last_cached = p + 2
        yield p


def modified_eratosthenes() -> Iterator[int]:
    # modified_eratosthenes taken, refactored from https://stackoverflow.com/a/19391111
    yield from (2, 3, 5, 7)
    sieve: Dict[int, int] = {}
    recurse = primes()
    next(recurse)
    prime = next(recurse)
    if prime != 3:
        raise ValueError()
    prime_squared = prime * prime
    for candidate in itertools.count(9, 2):
        if candidate in sieve:  # if c is a multiple of some base prime, or
            step = sieve.pop(candidate)
        elif candidate < prime_squared:  # prime, or
            yield candidate
            continue
        else:  # the next base prime's square
            if candidate != prime_squared:
                raise ValueError()
            step = prime * 2
            prime = next(recurse)
            prime_squared = prime * prime
        candidate += step  # the next multiple
        while candidate in sieve:  # make sure to not overwrite another sieve entry
            candidate += step
        sieve[candidate] = step


def prime_factors(num: int) -> Iterator[int]:
    if num < 0:
        yield -1
        num = -num
    if num == 0:
        yield 0
    else:
        root = math.ceil(math.sqrt(num))
        for factor in primes():
            modulo = num % factor
            if modulo == 0:
                while modulo == 0:  # double-check to call math.sqrt once
                    yield factor
                    num //= factor
                    modulo = num % factor
                root = math.ceil(math.sqrt(num))
            if num <= 1:
                break
            if factor > root:
                yield num
                break


def main() -> int:
    return max(prime_factors(600851475143))


if __name__ == '__main__':
    print(main())
