"""
Project Euler Problem 38

Once I found out where the end was, it seemed to be relatively easy

Problem:

Take the number 192 and multiply it by each of 1, 2, and 3:

    192 × 1 = 192
    192 × 2 = 384
    192 × 3 = 576

By concatenating each product we get the 1 to 9 pandigital, 192384576. We will
call 192384576 the concatenated product of 192 and (1,2,3)

The same can be achieved by starting with 9 and multiplying by 1, 2, 3, 4, and
5, giving the pandigital, 918273645, which is the concatenated product of 9 and
(1,2,3,4,5).

What is the largest 1 to 9 pandigital 9-digit number that can be formed as the
concatenated product of an integer with (1,2, ... , n) where n > 1?
"""
import functools
from typing import Tuple

import p0074


def main() -> int:
    answer: Tuple[int, ...] = (9, 1, 8, 2, 7, 3, 6, 4, 5)
    list_of_digits = list(range(1, 10))
    for num in range(1, 10000):
        cur_digits = tuple(p0074.digits(num))[::-1]
        for x in range(2, 9):
            if len(cur_digits) > 9:
                break
            elif any(cur_digits.count(digit) != 1 for digit in cur_digits):
                break
            elif len(cur_digits) == 9 and sorted(cur_digits) == list_of_digits:
                if cur_digits > answer:
                    print(num, cur_digits)
                    answer = cur_digits
                break
            cur_digits = cur_digits + tuple(p0074.digits(x * num))[::-1]
    return functools.reduce(lambda x, y: x * 10 + y, answer)


if __name__ == '__main__':
    print(main())
