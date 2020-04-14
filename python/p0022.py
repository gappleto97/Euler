"""
Project Euler Problem 22

I had to approach this by modifying the factors function from p0003, but it
seemed to work fairly well.

Problem:

Using names.txt (right click and 'Save Link/Target As...'), a 46K text file
containing over five-thousand first names, begin by sorting it into
alphabetical order. Then working out the alphabetical value for each name,
multiply this value by its alphabetical position in the list to obtain a name
score.

For example, when the list is sorted into alphabetical order, COLIN, which is
worth 3 + 15 + 12 + 9 + 14 = 53, is the 938th name in the list. So, COLIN would
obtain a score of 938 × 53 = 49714.

What is the total of all the name scores in the file?
"""
import pathlib


def score(name: str, idx: int) -> int:
    return sum(ord(x) & 0x3F for x in name) * idx


def main() -> int:
    with pathlib.Path(__file__).parent.parent.joinpath('_data', 'p0022_names.txt').open('r') as f:
        names = sorted(f.read().upper().replace('"', '').split(','))
    return sum(score(name, idx) for idx, name in enumerate(names, 1))


if __name__ == '__main__':
    print(main())
