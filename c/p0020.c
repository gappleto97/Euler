/*
Project Euler Problem 20

This was a nice problem to work on because it forced me to actually implement subtraction

Problem:

n! means n × (n − 1) × ... × 3 × 2 × 1

For example, 10! = 10 × 9 × ... × 3 × 2 × 1 = 3628800,
and the sum of the digits in the number 10! is 3 + 6 + 2 + 8 + 8 + 0 + 0 = 27.

Find the sum of the digits in the number 100!
*/
#include <stdio.h>
#include "include/bcd.h"

#ifdef DOXYGEN
namespace c::p0020 {
#endif


int main(int argc, char const *argv[])  {
    unsigned long long answer = 0;
    BCD_int factorial = new_BCD_int1(100);
    ifactorial_bcd(&factorial);
    for (size_t i = 0; i < factorial.bcd_digits; i++)   {
        answer += factorial.data[i] & 0x0F;
        answer += factorial.data[i] >> 4;
    }
    free_BCD_int(&factorial);
    printf("%llu", answer);
    return 0;
}

#ifdef DOXYGEN
}
#endif
