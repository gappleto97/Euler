/*
Project Euler Problem 4

This was pretty easy to do, given the digit infrastructure I already had built up

Problem:

A palindromic number reads the same both ways. The largest palindrome made from
the product of two 2-digit numbers is 9009 = 91 × 99.

Find the largest palindrome made from the product of two 3-digit numbers.
*/
#include <stdio.h>

#include "include/digits.h"

#ifdef DOXYGEN
namespace c::p0004 {
#endif


int main(int argc, char const *argv[])  {
    unsigned long long answer = 0, i, j, a, z, prod;
    bool broken;
    for (i = 100; i < 1000; i++)    {
        for (j = 100; j < 1000; j++)    {
            prod = i * j;
            digit_counter dc = digits(prod);
            broken = false;
            for (a = 0, z = dc.idx; a < z; a++, z--)    {
                if (dc.digits[a] != dc.digits[z])   {
                    broken = true;
                    break;
                }
            }
            if (!broken)    {
                answer = max(answer, prod);
            }
            free_digit_counter(dc);
        }
    }
    printf("%llu", answer);
    return 0;
}

#ifdef DOXYGEN
}
#endif
