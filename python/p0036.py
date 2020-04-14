"""
Project Euler Problem 36

This one ended up being very easy thanks to the datetime library

Problem:

The decimal number, 585 = 10010010012 (binary), is palindromic in both bases.

Find the sum of all numbers, less than one million, which are palindromic in
base 10 and base 2.

(Please note that the palindromic number, in either base, may not include
leading zeros.)

"""
import p0004


def bin_repr(x: int) -> str:
    return bin(x)[2:]


def main() -> int:
    answer = 0
    for x in range(1, 1000000):
        if p0004.is_palindrome(x) and p0004.is_palindrome(x, rep_func=bin_repr):
            answer += x
    return answer


if __name__ == '__main__':
    print(main())
