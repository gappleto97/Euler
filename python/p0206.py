"""
Project Euler Problem 206

This one ended up being a regex problem. I am sure there is a more clever way
to go about it, but this way works fairly quickly.

Problem:

Find the unique positive integer whose square has the form 1_2_3_4_5_6_7_8_9_0,
where each “_” is a single digit.
"""
import re


def main() -> int:
    pattern = re.compile("1.2.3.4.5.6.7.8.9.0")
    minimum = 1010101010  # sqrt of minimum number matching pattern
    maximum = 1389026624  # sqrt of maximum number matching pattern
    step = 10  # the square is a multiple of 10, which means it's a multiple of
    # 100, since 2 and 5 are both prime. Therefore we can step by at least 10
    for x in range(minimum, maximum, step):
        if pattern.match(repr(x * x)):
            return x
    return -1


if __name__ == '__main__':
    print(main())
