"""
Project Euler Problem 118

This one doesn't look pretty, but it was much faster to roll out my own loop than try to make combinations() work here.
It's also more efficient because it recycles some of the number generation.

Problem:

Using all of the digits 1 through 9 and concatenating them freely to form decimal integers, different sets can be
formed. Interestingly with the set {2,5,47,89,631}, all of the elements belonging to it are prime.

How many distinct sets containing each of the digits one through nine exactly once contain only prime elements?
"""
import itertools
import math

import p0003

all_digits = set("123456789")

max_digits = {
    2: 8,
    3: 7,
    4: 6,
    5: 5,
    6: 3,
}


def is_not_pandigital(*numbers: int) -> int:
    """If it's pandigital, returns 0. If not, returns the number of total digits"""
    string = "".join(str(n) for n in numbers)
    length = len(string)
    if length == 9 and set(string) == all_digits:
        return 0
    return length


def main() -> int:
    answer = 0

    def check(*numbers: int) -> bool:
        """Modifies answer if necessary, then returns True if you need to continue"""
        if numbers[-2] >= numbers[-1]:
            return True  # otherwise we can't guarantee uniqueness
        length = is_not_pandigital(*numbers)
        if length == 9:
            return True  # this means any nested loops can't be pandigital
        if not length:
            nonlocal answer
            answer += 1
            return True  # if this set is pandigital, skip nested loops
        return False

    cached_primes = tuple(p0003.primes(98765432))  # should be largest eligible number
    for a in itertools.takewhile((10**5).__gt__, cached_primes):
        a_digits = math.ceil(math.log10(a))
        for b in itertools.takewhile((10**(9 - a_digits)).__gt__, cached_primes):
            if check(a, b):
                continue
            b_digits = a_digits + math.ceil(math.log10(b))
            for c in itertools.takewhile((10**(9 - b_digits)).__gt__, cached_primes):
                if check(a, b, c):
                    continue
                c_digits = b_digits + math.ceil(math.log10(c))
                for d in itertools.takewhile((10**(9 - c_digits)).__gt__, cached_primes):
                    if check(a, b, c, d):
                        continue
                    d_digits = c_digits + math.ceil(math.log10(d))
                    for e in itertools.takewhile((10**(9 - d_digits)).__gt__, cached_primes):
                        if check(a, b, c, d, e):
                            continue
                        e_digits = d_digits + math.ceil(math.log10(e))
                        for f in itertools.takewhile((10**(9 - e_digits)).__gt__, cached_primes):
                            if check(a, b, c, d, e, f):
                                continue
    return answer


if __name__ == '__main__':
    print(main())
