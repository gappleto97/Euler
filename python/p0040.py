"""
Project Euler Problem 40

Problem:

An irrational decimal fraction is created by concatenating the positive integers:

0.12345678910(1)112131415161718192021...

It can be seen that the 12th digit of the fractional part is 1.

If dn represents the nth digit of the fractional part, find the value of the following expression.

d1 × d10 × d100 × d1000 × d10000 × d100000 × d1000000
"""
import functools
import itertools
import math
import operator


def main():
    digits_seen = 1
    interests = {1, 10, 100, 1_000, 10_000, 100_000, 1_000_000}
    queries = []
    for x in itertools.count(1):
        digits = int(math.log10(x)) + 1
        for idx in range(digits):
            if digits_seen + idx in interests:
                queries.append(int(str(x)[idx]))
                if len(queries) == len(interests):
                    print(queries)
                    return functools.reduce(operator.mul, queries, 1)
        digits_seen += digits


if __name__ == '__main__':
    print(main())
