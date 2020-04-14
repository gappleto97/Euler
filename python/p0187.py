"""
Project Euler Problem 187

I was able to chain this with a previous problem. Probably a suboptimal
solution because of it, but it felt prettier this way.

I was able to add a short-circuited fail case to the is_prime() method, though

Revision 1:

Switched to a set comprehension with caching. This means that I can remove it
from the slow list.

Revision 2:

Switch from set comprehension to factor filtering. Now factor pairs are guaranteed
to be unique. Takes it from straight n^2 to n^2/2, so that's (including generating
primes) about a 1/3 time reduction.

Problem:

A composite is a number containing at least two prime factors. For example,
15 = 3 × 5; 9 = 3 × 3; 12 = 2 × 2 × 3.

There are ten composites below thirty containing precisely two, not necessarily
distinct, prime factors: 4, 6, 9, 10, 14, 15, 21, 22, 25, 26.

How many composite integers, n < 10**8, have precisely two, not necessarily
distinct, prime factors?
"""
import itertools

import p0003


def main() -> int:
    ten_8 = 10**8
    cached_primes = tuple(p0003.primes(ten_8 // 2 + 1))
    return sum(
        1
        for idx, y in enumerate(cached_primes)
        for x in itertools.takewhile((ten_8 // y).__ge__, itertools.islice(cached_primes, idx + 1))
    )


if __name__ == '__main__':
    print(main())
