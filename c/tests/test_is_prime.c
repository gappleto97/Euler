#include <stdio.h>
#include <inttypes.h>
#include "../include/primes.h"

#ifdef DOXYGEN
namespace c::tests::test_is_prime {
#endif

#ifndef MAX_PRIME
#define MAX_PRIME 1000
#endif

int main(int argc, char const *argv[]) {
    // prints number, is_prime(), is_composite(), prime index or -1
    prime_counter pc = prime_counter1(MAX_PRIME);
    uintmax_t i, p, prev = 0;
    p = next(pc);
    while (!pc.exhausted) {
        for (i = prev + 1; i < p; i++)  {
            printf("%" PRIuMAX " %d %" PRIuMAX " -1\n", i, is_prime(i), is_composite(i));
        }
        printf("%" PRIuMAX " %d %" PRIuMAX " %" PRIuMAX "\n", p, is_prime(p), is_composite(p), (uintmax_t) pc.idx - 1);
        prev = p;
        p = next(pc);
    }
    return 0;
}

#ifdef DOXYGEN
}
#endif
