"""
Project Euler Question 1

I decided I wanted to take a functional approach on this one. It also uses only
lazy functions, so it should take minimal memory usage.

Revision 1:

I found a closed form solution that works in the general case, so it's about an
order of magnitude faster now. It no longer uses lazy functions

Problem:

If we list all the natural numbers below 10 that are multiples of 3 or 5, we
get 3, 5, 6 and 9. The sum of these multiples is 23.

Find the sum of all the multiples of 3 or 5 below 1000.
"""


def summation(up_to: int, factor: int) -> int:
    """In the general case, summation(N, 1) returns (N-1) * N // 2
    In cases where you want to multiply by only by, for instance, factors of 7 it works
    based on the principle that the valid range covers N//7 numbers, but all values are multiples of 7.
    This means the final solution ends up being ((N//7)-1) * (N//7) * 7 // 2
    """
    n = up_to // factor
    return (n + 1) * factor * n // 2


def main() -> int:
    return summation(999, 3) + summation(999, 5) - summation(999, 15)


if __name__ == '__main__':
    print(main())
