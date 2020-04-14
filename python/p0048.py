"""
Project Euler Problem 48

Problem:

The series, 1^1 + 2^2 + 3^3 + ... + 10^10 = 10405071317.

Find the last ten digits of the series, 1^1 + 2^2 + 3^3 + ... + 1000^1000.
"""


def main() -> int:
    ten_ten = 10**10
    return sum(pow(x, x, ten_ten) for x in range(1, 1001)) % ten_ten


if __name__ == '__main__':
    print(main())
