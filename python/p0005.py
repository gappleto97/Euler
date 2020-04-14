"""
Project Euler Problem 5

I solved this problem by testing all combinations of the various multiples. I
actually tried to solve this by hand before doing this, and it wasn't terribly
hard. The answer turns out to be (2**4 * 3**2 * 5 * 7 * 11 * 13 * 17 * 19)

Problem:

2520 is the smallest number that can be divided by each of the numbers from 1
to 10 without any remainder.

What is the smallest positive number that is evenly divisible by all of the
numbers from 1 to 20?
"""
import functools
import itertools
import operator


def main() -> int:
    group = range(1, 21)
    answer = 1_000_000_000_000
    for x in group:
        for multiples in itertools.combinations(group, x):
            num = functools.reduce(operator.mul, multiples, 1)
            if num < answer and all(num % divisor == 0 for divisor in group):
                answer = num
    return answer


if __name__ == '__main__':
    print(main())
