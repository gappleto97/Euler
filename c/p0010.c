/*
Project Euler Problem 10

This is where my prime infrastructure shows its weak points, since the C version doesn't have a sieve.

Problem:

The sum of the primes below 10 is 2 + 3 + 5 + 7 = 17.

Find the sum of all the primes below two million.
*/
#include <stdio.h>
#include "include/primes.h"

#ifdef DOXYGEN
namespace c::p0010 {
#endif


int main(int argc, char const *argv[])  {
    unsigned long long tmp, answer = 0;
    prime_sieve ps = prime_sieve0();
    while ((tmp = next(ps)) < 2000000)  {
        answer += tmp;
    }
    free_prime_sieve(ps);
    printf("%llu", answer);  // this is because of a bug
    return 0;
}

#ifdef DOXYGEN
}
#endif
