"""
Project Euler Problem 10

This one was also relatively easy, but the problem I am running into is that my
original implementation of primes() is not very fast. I did go back and
refactor that, but I think I could do better. Maybe I will pass these through
cython at some point.

Revision 1:

I took the lambda that was previously being used and replaced it with a
partial() of an operator. This had a mild speed boost, and avoided using gross
lambdas.

Revision 2:

Now it just references the __gt__ function directly

Problem:

The sum of the primes below 10 is 2 + 3 + 5 + 7 = 17.

Find the sum of all the primes below two million.
"""
import itertools

import p0003


def main() -> int:
    return sum(itertools.takewhile((2_000_000).__gt__, p0003.primes()))


if __name__ == '__main__':
    print(main())
