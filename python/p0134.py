"""
Project Euler Problem 134

Officially I have this solved, but it took 76 core-minutes, which is just not a
reasonable runtime. I have made some minor optimizations in the hopes that it
will be reduced further, but it only went down to 37 core-minutes. Not good
enough.

Revision 1:

Found Chinese Remainder Theorem, applied it, got infinity x speed boost

Problem:

Consider the consecutive primes p1 = 19 and p2 = 23. It can be verified that
1219 is the smallest number such that the last digits are formed by p1 whilst
also being divisible by p2.

In fact, with the exception of p1 = 3 and p2 = 5, for every pair of consecutive
primes, p2 > p1, there exist values of n for which the last digits are formed
by p1 and n is divisible by p2. Let S be the smallest of these values of n.

Find ∑ S for every pair of consecutive primes with 5 ≤ p1 ≤ 1000000.
"""
import p0003


def mul_inv(a: int, b: int) -> int:
    """Multiplicative inverse for modulo numbers"""
    if b == 1:
        return 1
    b0: int = b
    x0: int = 0
    x1: int = 1
    while a > 1:
        q: int = a // b
        a, b = b, a % b
        x0, x1 = x1 - q * x0, x0
    if x1 < 0:
        return x1 + b0
    return x1


def main() -> int:
    answer: int = 0
    pow_10: int = 10
    iterator = p0003.primes(1000005)  # iterate over primes <1000005
    next(iterator)  # skip 2
    next(iterator)  # skip 3
    p1: int = next(iterator)  # p1 = 5
    for p2 in iterator:  # 7, 11, 13, 17, ...
        while pow_10 < p1:
            pow_10 *= 10
        answer += (p1 * p2 * mul_inv(p2, pow_10)) % (p2 * pow_10)
        # simplified Chinese Remainder Theorem
        p1 = p2
    return answer


if __name__ == '__main__':
    print(main())
