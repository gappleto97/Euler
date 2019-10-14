"""
Project Euler Problem 71

This ended up being mostly a filtering problem. Getting the considered set
small enough to run in a reasonable time took a bit. The actual problem is easy

Problem:

Consider the fraction, n/d, where n and d are positive integers. If n<d and
HCF(n,d)=1, it is called a reduced proper fraction.

If we list the set of reduced proper fractions for d ≤ 8 in ascending order of
size, we get:

1/8, 1/7, 1/6, 1/5, 1/4, 2/7, 1/3, 3/8, 2/5, 3/7, 1/2, 4/7, 3/5, 5/8, 2/3, 5/7,
3/4, 4/5, 5/6, 6/7, 7/8

It can be seen that 2/5 is the fraction immediately to the left of 3/7.

By listing the set of reduced proper fractions for d ≤ 1,000,000 in ascending
order of size, find the numerator of the fraction immediately to the left of
3/7.
"""
import fractions


def main() -> int:
    return max(
        fractions.Fraction(x, y)
        for y in range(1, 1_000_000)
        for x in range((y - 1) * 3 // 7, y * 3 // 7)
    ).numerator


if __name__ == '__main__':
    print(main())
