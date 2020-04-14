"""
Project Euler Problem 20

If I need to do bigger numbers than this, or multiple times, a recursive
approach will almost certainly be better. This works for now, though.

Problem:

n! means n × (n − 1) × ... × 3 × 2 × 1

For example, 10! = 10 × 9 × ... × 3 × 2 × 1 = 3628800,
and the sum of the digits in the number 10! is 3 + 6 + 2 + 8 + 8 + 0 + 0 = 27.

Find the sum of the digits in the number 100!
"""
import math


def main() -> int:
    return sum(int(x) for x in repr(math.factorial(100)))


if __name__ == '__main__':
    print(main())
