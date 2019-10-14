"""
Project Euler Problem 87

I ended up having to do this with recursion, which I normally do not like to
use that much. Small changes can have large effects on later results. Still,
this seems to work for the moment.

Revision 1:

Shorten the code slightly to take advantage of the primes() stop argument.
Probably loses performance slightly.

Problem:

The smallest number expressible as the sum of a prime square, prime cube, and
prime fourth power is 28. In fact, there are exactly four numbers below fifty
that can be expressed in such a way:

28 = 2**2 + 2**3 + 2**4
33 = 3**2 + 2**3 + 2**4
49 = 5**2 + 2**3 + 2**4
47 = 2**2 + 3**3 + 2**4

How many numbers below fifty million can be expressed as the sum of a prime
square, prime cube, and prime fourth power?
"""
import p0003


def main() -> int:
    seen = set()
    root2_50M = int(50_000_000 ** (1/2)) + 1
    root3_50M = int(50_000_000 ** (1/3)) + 1
    root4_50M = int(50_000_000 ** (1/4)) + 1
    for x in p0003.primes(root2_50M):
        x2 = x * x
        for y in p0003.primes(root3_50M):
            y3 = y * y * y
            for z in p0003.primes(root4_50M):
                total = x2 + y3 + z * z * z * z
                if total < 50_000_000:
                    seen.add(total)
                else:
                    break
    return len(seen)


if __name__ == '__main__':
    print(main())
