"""
Project Euler Problem 42

The nth term of the sequence of triangle numbers is given by, tn = ½n(n+1); so the first ten triangle numbers are:

1, 3, 6, 10, 15, 21, 28, 36, 45, 55, ...

By converting each letter in a word to a number corresponding to its alphabetical position and adding these values we
form a word value. For example, the word value for SKY is 19 + 11 + 25 = 55 = t10. If the word value is a triangle
number then we shall call the word a triangle word.

Using words.txt (right click and 'Save Link/Target As...'), a 16K text file containing nearly two-thousand common
English words, how many are triangle words?
"""
import pathlib
from typing import Set


class Cache:
    cache: Set[int] = set()
    max_cached = 0
    max_cached_idx = -1


def triangle(n: int) -> int:
    return n * (n + 1) // 2


def is_in_triangle(n: int) -> bool:
    while n > Cache.max_cached:
        Cache.max_cached_idx += 1
        Cache.max_cached = triangle(Cache.max_cached_idx)
        Cache.cache.add(Cache.max_cached)
    return n in Cache.cache


def main():
    answer = 0
    with pathlib.Path(__file__).parent.parent.joinpath('_data', 'p0042_words.txt').open('rb') as f:
        words = f.read().replace(b'"', b'').split(b',')
    for word in words:
        value = sum(x - 64 for x in word)
        if is_in_triangle(value):
            answer += 1
    return answer


if __name__ == '__main__':
    print(main())
