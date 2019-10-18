#ifndef EULER_MATH_H
#define EULER_MATH_H

#ifdef DOXYGEN
namespace c::include::math {
#endif

#include "./macros.h"

#include <stdlib.h>
#include <stdint.h>

uintmax_t factorial(unsigned int n);
inline uintmax_t factorial(unsigned int n)  {
    // note that this function only works for numbers smaller than MAX_FACTORIAL_64
    if ((sizeof(uintmax_t) == 8 && n > MAX_FACTORIAL_64) || (sizeof(uintmax_t) == 16 && n > MAX_FACTORIAL_128))
        return -1;
    uintmax_t ret = 1;
    for (unsigned int i = 2; i <= n; ++i) {
        ret *= i;
    }
    return ret;
}

uintmax_t n_choose_r(unsigned int n, unsigned int r)    {
    // function returns -1 if it overflows
    if ((sizeof(uintmax_t) == 8 && n <= MAX_FACTORIAL_64) || (sizeof(uintmax_t) == 16 && n <= MAX_FACTORIAL_128))   {
        // fast path if small enough
        return factorial(n) / factorial(r) / factorial(n-r);
    }
    // slow path for larger numbers
    int *factors;
    uintmax_t answer, tmp;
    unsigned int i, j;
    factors = (int *) malloc(sizeof(int) * (n + 1));
    // collect factors of final number
    for (i = 2; i <= n; i++) {
        factors[i] = 1;
    }
    // negative factor values indicate need to divide
    for (i = 2; i <= r; i++) {
        factors[i] -= 1;
    }
    for (i = 2; i <= n - r; i++) {
        factors[i] -= 1;
    }
    // this loop reduces to prime factors only
    for (i = n; i > 1; i--) {
        for (j = 2; j < i; j++) {
            if (i % j == 0) {
                factors[j] += factors[i];
                factors[i / j] += factors[i];
                factors[i] = 0;
                break;
            }
        }
    }
    i = j = 2;
    answer = 1;
    while (i <= n)  {
        while (factors[i] > 0)  {
            tmp = answer;
            answer *= i;
            while (answer < tmp && j <= n)  {
                while (factors[j] < 0)  {
                    tmp /= j;
                    factors[j]++;
                }
                j++;
                answer = tmp * i;
            }
            if (answer < tmp)   {
                return -1;  // this indicates an overflow
            }
            factors[i]--;
        }
        i++;
    }
    while (j <= n)  {
        while (factors[j] < 0)  {
            answer /= j;
            factors[j]++;
        }
        j++;
    }
    free(factors);
    return answer;
}

#ifdef DOXYGEN
};
#endif

#endif
