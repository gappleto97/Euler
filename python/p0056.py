"""
Project Euler Problem 56

Problem:

A googol (10100) is a massive number: one followed by one-hundred zeros; 100100 is almost unimaginably large: one
followed by two-hundred zeros. Despite their size, the sum of the digits in each number is only 1.

Considering natural numbers of the form, ab, where a, b < 100, what is the maximum digital sum?
"""
import p0074


def main() -> int:
    answer = 0
    for x in range(2, 100):
        for y in range(1, 100):
            answer = max((answer, sum(p0074.digits(x**y))))
    return answer


if __name__ == '__main__':
    print(main())
