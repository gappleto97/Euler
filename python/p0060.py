"""
Project Euler Problem 60

Problem:

The primes 3, 7, 109, and 673, are quite remarkable. By taking any two primes and concatenating them in any order the
result will always be prime. For example, taking 7 and 109, both 7109 and 1097 are prime. The sum of these four primes,
792, represents the lowest sum for a set of four primes with this property.

Find the lowest sum for a set of five primes for which any two primes concatenate to produce another prime.
"""
from collections import defaultdict
from itertools import combinations
from typing import DefaultDict, List, Set

from p0003 import primes
from p0007 import is_prime


def is_concat_prime(x: int, y: int) -> bool:
    sx = str(x)
    sy = str(y)
    return is_prime(int(sx + sy)) and is_prime(int(sy + sx))


def main() -> int:
    iterator = primes()
    cached_primes: List[int] = []
    for p in iterator:
        cached_primes.append(p)
        if p > 4:
            break
    cached_primes.remove(2)
    # 2 is excluded because higher even numbers can't be prime
    cached_primes.remove(5)
    # 5 is excluded because if a number ends with 5, it's divisible by 5
    compat: DefaultDict[int, Set[int]] = defaultdict(set)
    for x in iterator:
        for y in cached_primes:
            if is_concat_prime(x, y):
                for triplet in combinations(compat[y], 3):
                    if all(a in compat[b] for a, b in combinations(triplet, 2)):
                        if all(is_concat_prime(x, a) for a in triplet):
                            return x + y + sum(triplet)
                compat[x].add(y)
                compat[y].add(x)
        cached_primes.append(x)
    return -1


if __name__ == '__main__':
    print(main())
