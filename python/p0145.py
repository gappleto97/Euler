"""
Project Euler Problem 145

For some reason there are no solutions above 10**8, so you can reduce the search space by a lot. In addition, you never
need to check numbers divisble by 10, reducing it further. I think I've narrowed the valid search space to ~4.5% of the
one given by the problem.

Revision 1:

Turns out that using the built-in repr function makes this go much (~5x) faster, so I'll switch to that until I find a
narrower solution. It's very nearly at the 60s mark now.

Revision 2:

I reduced the search space by trial and error, getting it down to ~3.15% of the original

Problem:

Some positive integers n have the property that the sum [ n + reverse(n) ] consists entirely of odd (decimal) digits.
For instance, 36 + 63 = 99 and 409 + 904 = 1313. We will call such numbers reversible; so 36, 63, 409, and 904 are
reversible. Leading zeroes are not allowed in either n or reverse(n).

There are 120 reversible numbers below one-thousand.

How many reversible numbers are there below one-billion (10^9)?
"""
import itertools
from typing import Set


def main() -> int:
    seen: Set[int] = set()
    seen_update = seen.update
    odd_digits = {"1", "3", "5", "7", "9"}
    for x in itertools.chain.from_iterable(range(x, 10**8 // 2, 10) for x in (2, 4, 5, 6, 7, 8)):
        if x in seen:
            continue
        inverse = int(repr(x)[::-1])
        if all(digit in odd_digits for digit in repr(x + inverse)):
            seen_update((x, inverse))
    return len(seen)


if __name__ == '__main__':
    print(main())
