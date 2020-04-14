/*
Project Euler Problem 6

I know there is a closed form solution to sum of squares, but I didn't want to
cheat and look it up. I was able to remember the closed form formula for sum of
natural numbers, though, so this is still pretty fast.

Problem:

The sum of the squares of the first ten natural numbers is,
1**2 + 2**2 + ... + 10**2 = 385

The square of the sum of the first ten natural numbers is,
(1 + 2 + ... + 10)**2 = 55**2 = 3025

Hence the difference between the sum of the squares of the first ten natural
numbers and the square of the sum is 3025 − 385 = 2640.

Find the difference between the sum of the squares of the first one hundred
natural numbers and the square of the sum.
*/
#include <stdio.h>

#ifdef DOXYGEN
namespace c::p0006 {
#endif


int main(int argc, char const *argv[])  {
    unsigned long long sum = 100 * 101 / 2, sum_of_squares = 0;
    for (unsigned long long i = 1; i < 101; i++)    {
        sum_of_squares += i * i;
    }
    printf("%llu", sum * sum - sum_of_squares);
    return 0;
}

#ifdef DOXYGEN
}
#endif
