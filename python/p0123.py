"""
Project Euler Problem 123

Brute force, with minor optimizations, seems to have worked here. Using cython
shaved a few seconds off the runtime, but not enough to be hugely noticable.
Additionally, I narrowed the search range by figuring that the remainder has to
take place after the prime is 10**5.

Revision 1:

Reduce search space further by remembering that it's unlikely for n % (10**10 + epsilon) >= 10**10

Problem:

Let p[n] be the nth prime: 2, 3, 5, 7, 11, ..., and let r be the remainder when
(p[n]−1)**n + (p[n]+1)**n is divided by p[n]**2.

For example, when n = 3, p[3] = 5, and 4**3 + 6**3 = 280 ≡ 5 mod 25.

The least value of n for which the remainder first exceeds 10**9 is 7037.

Find the least value of n for which the remainder first exceeds 10**10.
"""
import p0003


def main() -> int:
    min_p = 235_000
    ten_ten = 10**10
    for n, p in enumerate(p0003.primes(), 1):
        if p < min_p or n & 1 == 0:  # Seems to always produce remainder of 2?
            continue
        base = ((p-1)**n + (p+1)**n)
        if base < ten_ten:
            continue
        elif base % (p * p) > ten_ten:
            return n
    return -1


if __name__ == '__main__':
    print(main())
