"""
Project Euler Problem 19

This one ended up being very easy thanks to the datetime library

Problem:

You are given the following information, but you may prefer to do some research
for yourself.

    1 Jan 1900 was a Monday.
    Thirty days has September,
    April, June and November.
    All the rest have thirty-one,
    Saving February alone,
    Which has twenty-eight, rain or shine.
    And on leap years, twenty-nine.
    A leap year occurs on any year evenly divisible by 4, but not on a century
    unless it is divisible by 400.

How many Sundays fell on the first of the month during the twentieth century
(1 Jan 1901 to 31 Dec 2000)?
"""
import datetime


def monthly_iterator(years, months=range(1, 13), day=1):
    for x in years:
        for y in months:
            date = datetime.date(x, y, day)
            if date.weekday() == 6:
                yield date


def main() -> int:
    return sum(1 for _ in monthly_iterator(years=range(1901, 2001)))


if __name__ == '__main__':
    print(main())
