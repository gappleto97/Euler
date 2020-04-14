"""
Project Euler Problem 49

I was surprised how fast my brute-force solution went, but it worked

Problem:

The arithmetic sequence, 1487, 4817, 8147, in which each of the terms increases
by 3330, is unusual in two ways: (i) each of the three terms are prime, and,
(ii) each of the 4-digit numbers are permutations of one another.

There are no arithmetic sequences made up of three 1-, 2-, or 3-digit primes,
exhibiting this property, but there is one other 4-digit increasing sequence.

What 12-digit number do you form by concatenating the three terms in this
sequence?
"""
import itertools

import p0003
import p0074


def main() -> int:
    primes_at_1000 = p0003.primes()
    for p in primes_at_1000:
        if p > 1000:
            break
    primes_at_1000, pgen_1 = itertools.tee(primes_at_1000)
    for p1 in pgen_1:
        if p1 == 1487:
            continue
        pgen_1, pgen_2 = itertools.tee(pgen_1)
        for p2 in pgen_2:
            if p2 > 10000 - p1 // 2:
                break
            pgen_2, pgen_3 = itertools.tee(pgen_2)
            for p3 in pgen_3:
                if p3 > 10000:
                    break
                elif p1 - p2 < p2 - p3:
                    continue
                elif p1 - p2 > p2 - p3:
                    break
                elif set(p0074.digits(p1)) == set(p0074.digits(p2)) == set(p0074.digits(p3)):
                    return p1 * 10**8 + p2 * 10**4 + p3
    return -1


if __name__ == '__main__':
    print(main())
