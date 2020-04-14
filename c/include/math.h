#ifndef EULER_MATH_H
#define EULER_MATH_H

#ifdef DOXYGEN
namespace c::include::math {
#endif

#include "./macros.h"

#include <stdlib.h>
#include <stdint.h>

uintmax_t factorial(unsigned char n);
inline uintmax_t factorial(unsigned char n) {
    // note that this function only works for numbers smaller than MAX_FACTORIAL_64
    if ((sizeof(uintmax_t) == 8 && n > MAX_FACTORIAL_64) || (sizeof(uintmax_t) == 16 && n > MAX_FACTORIAL_128))
        return -1;
    uintmax_t ret = 1;
    for (unsigned char i = 2; i <= n; ++i)  {
        ret *= i;
    }
    return ret;
}

uintmax_t n_choose_r(const unsigned int n, const unsigned int r)    {
    // function returns -1 if it overflows, -2 if it can't allocate memory, or 0 if n < r
    if (n < r)
        return 0;
    if ((sizeof(uintmax_t) == 8 && n <= MAX_FACTORIAL_64) || (sizeof(uintmax_t) == 16 && n <= MAX_FACTORIAL_128))   {
        // fast path if small enough
        return factorial(n) / factorial(r) / factorial(n-r);
    }
    // slow path for larger numbers
    #if CL_COMPILER
        signed char *factors = (signed char *) malloc(sizeof(signed char) * (n + 1));
        if (factors == NULL)
            return -2;
    #else
        signed char factors[n+1];
    #endif
    // collect factors of final number
    // negative factor values indicate need to divide
    // note that the highest absolute value in factors would be 127 if sizeof(uintmax_t) == 16
    for (unsigned int i = r + 1; i <= n; i++)
        factors[i] = 1;
    for (unsigned int i = 2; i <= r; i++)
        factors[i] = 0;  // we can set directly to 0 because r <= n, so they would cancel completely
    for (unsigned int i = 2; i <= n - r; i++)
        factors[i]--;  // we have to decrement because we don't know if (n - r) <= r
    // this loop reduces to prime factors only, which helps cancel some
    for (unsigned int i = n; i > 1; i--)    {
        for (unsigned int j = 2; j < i; j++)    {
            if (i % j == 0) {
                factors[j] += factors[i];
                factors[i / j] += factors[i];
                factors[i] = 0;
                break;
            }
        }
    }
    uintmax_t answer = 1;
    unsigned int div_idx = 2;
    for (unsigned int mul_idx = 2; mul_idx <= n; mul_idx++) {
        while (factors[mul_idx] > 0)    {
            uintmax_t prev = answer;  // store the previous value to check for overflow
            answer *= mul_idx;
            for (; answer < prev && div_idx <= n; div_idx++)    {
                // if there was an overflow, and there are still divisor candidates, attempt to fix it
                while (factors[div_idx] < 0)    {
                    prev /= div_idx;
                    factors[div_idx]++;
                }
                answer = prev * mul_idx;
            }
            if (answer < prev)  {
                // if you couldn't fix it, return the overflow error code
                #if CL_COMPILER
                    free(factors);
                #endif
                return -1;
            }
            factors[mul_idx]--;
        }
    }
    for (; div_idx <= n; div_idx++) {
        while (factors[div_idx] < 0)  {
            answer /= div_idx;
            factors[div_idx]++;
        }
    }
    #if CL_COMPILER
        free(factors);
    #endif
    return answer;
}

#ifdef DOXYGEN
};
#endif

#endif
