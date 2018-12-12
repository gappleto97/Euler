"""
Project Euler Problem 123

Brute force, with minor optimizations, seems to have worked here. Using cython
shaved a few seconds off the runtime, but not enough to be hugely noticable.
Additionally, I narrowed the search range by figuring that the remainder has to
take place after the prime is 10**5.

Problem:

Let p[n] be the nth prime: 2, 3, 5, 7, 11, ..., and let r be the remainder when
(p[n]−1)**n + (p[n]+1)**n is divided by p[n]**2.

For example, when n = 3, p[3] = 5, and 4**3 + 6**3 = 280 ≡ 5 mod 25.

The least value of n for which the remainder first exceeds 10**9 is 7037.

Find the least value of n for which the remainder first exceeds 10**10.
"""
import cython

from p0007 import primes


@cython.cfunc
@cython.locals(
    ten_five=cython.ulonglong,
    ten_ten=cython.ulonglong,
    n=cython.ulonglong,
    p=cython.ulonglong
)
@cython.returns(cython.ulonglong)
def main() -> int:
    ten_five = 10**5
    ten_ten = 10**10
    for n, p in enumerate(primes(), 1):
        if p < ten_five:
            continue
        elif n & 1 == 0:  # Seems to always produce remainder of 2?
            continue
        elif ((p-1)**n + (p+1)**n) % p**2 > ten_ten:
            return n


if __name__ == '__main__':
    print(main())