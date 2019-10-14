"""
Project Euler Problem 4

I couldn't figure out how to do this as efficiently as I would have liked. I am
SURE that there is a better way to check if a number is a palindrome, but I
could not think of one.

Revision 1:

I changed is_palindrome to take in any repr function that you care to give it

Problem:

A palindromic number reads the same both ways. The largest palindrome made from
the product of two 2-digit numbers is 9009 = 91 × 99.

Find the largest palindrome made from the product of two 3-digit numbers.

"""
import itertools
from typing import Callable


def is_palindrome(n, rep_func: Callable[[int], str] = repr):
    """Checks if the string representation of an object is a palindrome"""
    r = rep_func(n)
    return r == r[::-1]


def main():
    result = 0
    for x, y in itertools.combinations(range(100, 1000), 2):
        num = x * y
        if num > result and is_palindrome(num):
            result = num
    return result


if __name__ == '__main__':
    print(main())
