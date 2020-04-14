"""
Project Euler Problem 34

This ended up being a filtering problem. The problem with my solution is that I
am not satisfied with my filter at all. I feel like there is a more efficient
way to go about it.

Problem:

145 is a curious number, as 1! + 4! + 5! = 1 + 24 + 120 = 145.

Find the sum of all numbers which are equal to the sum of the factorial of
their digits.

Note: as 1! = 1 and 2! = 2 are not sums they are not included.
"""
import math


def main() -> int:
    answer = 0
    for x in range(10, 100000):
        if sum(math.factorial(int(y)) for y in repr(x)) == x:
            answer += x
    return answer


if __name__ == '__main__':
    print(main())
