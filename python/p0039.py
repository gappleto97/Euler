"""
Project Euler Problem 39

Problem:

If p is the perimeter of a right angle triangle with integral length sides, {a,b,c}, there are exactly three solutions
for p = 120.

{20,48,52}, {24,45,51}, {30,40,50}

For which value of p ≤ 1000, is the number of solutions maximised?
"""


def main() -> int:
    biggest = 0
    biggest_size = 0
    for p in range(3, 1000):
        solutions = set()
        for c in range(3, p):
            for b in range(1, (p - c) // 2 + 1):
                a = p - b - c
                if a > c or b > c:
                    continue
                elif a*a + b*b == c*c:
                    solutions.add(frozenset((a, b, c)))
        num_solutions = len(solutions)
        if num_solutions > biggest_size:
            biggest_size = num_solutions
            biggest = p
    return biggest


if __name__ == '__main__':
    print(main())
