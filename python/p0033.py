"""
Project Euler Problem 33

This ended up being a filtering problem. The problem with my solution is that I
am not satisfied with my filter at all. I feel like there is a more efficient
way to go about it.

Problem:

The fraction 49/98 is a curious fraction, as an inexperienced mathematician in
attempting to simplify it may incorrectly believe that 49/98 = 4/8, which is
correct, is obtained by cancelling the 9s.

We shall consider fractions like, 30/50 = 3/5, to be trivial examples.

There are exactly four non-trivial examples of this type of fraction, less
than one in value, and containing two digits in the numerator and denominator.

If the product of these four fractions is given in its lowest common terms,
find the value of the denominator.
"""
import fractions
import itertools


def main() -> int:
    answer = fractions.Fraction(1, 1)
    counter = 0
    for num, denom in itertools.combinations(range(10, 100), 2):
        frac = fractions.Fraction(num, denom)
        if frac < 1:
            rnum = repr(num)
            rdenom = repr(denom)
            if any(x in rdenom for x in rnum):
                if not any(x == y for x, y in zip(rnum, rdenom)):
                    if any(fractions.Fraction(int(rnum[x]), int(rdenom[y])) == frac
                           for x, y in itertools.combinations(range(len(rnum)), 2)
                           if rdenom[y] != "0"):
                        answer *= frac
                        counter += 1
    return answer.denominator


if __name__ == '__main__':
    print(main())
