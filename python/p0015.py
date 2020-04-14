"""
Project Euler Problem 15

Turns out this is easy, if you think sideways a bit

You can only go down or right. If we say right=1, then you can only have 20 1s, since otherwise you go off the grid.
You also can't have fewer than 20 1s, since then you go off the grid the other way. This means you can look at it as a
bit string, and the number of 40-bit strings with 20 1s is 40c20.

Problem:

Starting in the top left corner of a 2×2 grid, and only being able to move to the right and down, there are exactly 6
routes to the bottom right corner.

How many such routes are there through a 20×20 grid?

"""
import math


def n_choose_r(n, r):
    return math.factorial(n) // math.factorial(r) // math.factorial(n - r)


def lattice_paths(height: int, width: int):
    return n_choose_r(height + width, height)


def main():
    return lattice_paths(20, 20)


if __name__ == '__main__':
    print(main())
