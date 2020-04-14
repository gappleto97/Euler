"""
Project Euler Problem 30

This one was tricky because the last number was significantly higher than I
thought it would be.

Problem:

Surprisingly there are only three numbers that can be written as the sum of
fourth powers of their digits:

    1634 = 1**4 + 6**4 + 3**4 + 4**4
    8208 = 8**4 + 2**4 + 0**4 + 8**4
    9474 = 9**4 + 4**4 + 7**4 + 4**4

As 1 = 1**4 is not a sum it is not included.

The sum of these numbers is 1634 + 8208 + 9474 = 19316.

Find the sum of all the numbers that can be written as the sum of fifth powers
of their digits.
"""


def main() -> int:
    ret = 0
    for x in range(2, 10**6):
        if x == sum(int(y)**5 for y in repr(x)):
            print(x)
            ret += x
    return ret


if __name__ == '__main__':
    print(main())
