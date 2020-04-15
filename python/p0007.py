"""
Project Euler Problem 7

I was able to chain this with a previous problem. Probably a suboptimal
solution because of it, but it felt prettier this way.

I was able to add a short-circuited fail case to the is_prime() method, though

Revision 1:

Add a shortcut on is_prime to check the pfs cache first, and refactored to work
with the new pfs object and name

Revision 2:

Add a count to the trigger on prime numbers

Revision 3:

Add a stop parameter to primes()

Revision 4:

Switch to takewhile, use prime cache again

Revision 5:

Move primes() to p0003 in order to fix caching

Problem:

By listing the first six prime numbers: 2, 3, 5, 7, 11, and 13, we can see that
the 6th prime is 13.

What is the 10 001st prime number?
"""
from typing import Callable, Collection, Optional

import p0003


def is_prime(
    num: int,
    count: int = 1,
    distinct: bool = False
) -> bool:
    """Detects if a number is prime or not.

    If a count other than 1 is given, it returns True only if the number has
    exactly count prime factors."""
    if num in (0, 1):
        return False
    factors = iter(p0003.prime_factors(num))
    if count == 1:
        if num in p0003.cache:  # always has 2
            return True
        if num % 2 == 0:
            return False
        return next(factors) == num
    seen: Collection[Optional[int]]
    seen_add: Callable[[Optional[int]], None]
    if distinct:
        seen = set()
        seen_add = seen.add
    else:
        seen = []
        seen_add = seen.append
    while None not in seen and count != len(seen):
        seen_add(next(factors, None))
    return None not in seen and next(factors, None) is None


def main() -> int:
    for idx, num in enumerate(p0003.primes(), 1):
        if idx == 10001:
            return num
    return -1


if __name__ == '__main__':
    print(main())
