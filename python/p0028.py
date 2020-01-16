"""
Project Euler Problem 28

1       ends at 1^2
2-9     ends at 3^2
10-25   ends at 5^2
26-49   ends at 7^2

perimeter[0] = (1, )
perimeter[i] = ((2 * i - 1)^2, (2 * i + 1)^2]

i = 1
2 3 4 5 6 7 8 9
  ^   ^   ^   ^
2i - 1, 2 * 2i - 1, 3 * 2i - 1, 4 * 2i - 1

i = 2
10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25
          ^           ^           ^           ^
2i - 1, 2 * 2i - 1, 3 * 2i - 1, 4 * 2i - 1

i = 3
26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49
                ^                 ^                 ^                 ^

the corners are:
perimeter[i][x * 2i - 1 for x in (1, 2, 3, 4)]

Problem:

Starting with the number 1 and moving to the right in a clockwise direction a 5 by 5 spiral is formed as follows:

21 22 23 24 25
20  7  8  9 10
19  6  1  2 11
18  5  4  3 12
17 16 15 14 13

It can be verified that the sum of the numbers on the diagonals is 101.

What is the sum of the numbers on the diagonals in a 1001 by 1001 spiral formed in the same way?
"""


def square_corner_sum(i: int) -> int:
    """Given the index of some spiral, return the sum of its corners"""
    if i == 0:
        return 1
    r = range((2 * i - 1)**2 + 1, (2 * i + 1)**2 + 1)
    return sum(r[x * 2 * i - 1] for x in (1, 2, 3, 4))


def spiral_diagonal_sum(size: int) -> int:
    n_size = (size + 1) // 2  # change from square length to number of spirals
    return sum(square_corner_sum(x) for x in range(n_size))


def main() -> int:
    return spiral_diagonal_sum(1001)


if __name__ == '__main__':
    print(main())
