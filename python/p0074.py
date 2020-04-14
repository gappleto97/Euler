"""
Project Euler Problem 74

This ended up being a filtering problem. The problem with my solution is that I
am not satisfied with my filter at all. I feel like there is a more efficient
way to go about it.

Problem:

The number 145 is well known for the property that the sum of the factorial of
its digits is equal to 145:

1! + 4! + 5! = 1 + 24 + 120 = 145

Perhaps less well known is 169, in that it produces the longest chain of
numbers that link back to 169; it turns out that there are only three such
loops that exist:

169 → 363601 → 1454 → 169
871 → 45361 → 871
872 → 45362 → 872

It is not difficult to prove that EVERY starting number will eventually get
stuck in a loop. For example,

69 → 363600 → 1454 → 169 → 363601 (→ 1454)
78 → 45360 → 871 → 45361 (→ 871)
540 → 145 (→ 145)

Starting with 69 produces a chain of five non-repeating terms, but the longest
non-repeating chain with a starting number below one million is sixty terms.

How many chains, with a starting number below one million, contain exactly
sixty non-repeating terms?
"""
import math
from typing import Iterator, Set

cache = [math.factorial(x) for x in range(10)]


def digits(x: int) -> Iterator[int]:
    while x:
        x, y = divmod(x, 10)
        yield y


def main() -> int:
    paths = {
        69: 363600,
        78: 45360,
        145: 145,
        169: 363601,
        540: 145,
        871: 45361,
        872: 45362,
        1454: 169,
        45360: 871,
        45361: 871,
        45362: 872,
        363601: 1454,
        163600: 1454
    }
    answer = 0
    seen: Set[int] = set()
    for x in range(3, 1000000):
        while x not in seen:
            seen.add(x)
            if x in paths:
                x = paths[x]
            else:
                paths[x] = x = sum(cache[y] for y in digits(x))
        if len(seen) == 60:
            answer += 1
        seen.clear()
    return answer


if __name__ == '__main__':
    print(main())
