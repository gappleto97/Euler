/*
Project Euler Problem 12



Problem:

The sequence of triangle numbers is generated by adding the natural numbers. So
the 7th triangle number would be 1 + 2 + 3 + 4 + 5 + 6 + 7 = 28. The first ten
terms would be:

1, 3, 6, 10, 15, 21, 28, 36, 45, 55, ...

Let us list the factors of the first seven triangle numbers:

     1: 1
     3: 1,3
     6: 1,2,3,6
    10: 1,2,5,10
    15: 1,3,5,15
    21: 1,3,7,21
    28: 1,2,4,7,14,28

We can see that 28 is the first triangle number to have over five divisors.

What is the value of the first triangle number to have over five hundred
divisors?
*/
#include <stdio.h>
#include "include/factors.h"

#ifdef DOXYGEN
namespace c::p0012 {
#endif

typedef struct triangle_iterator triangle_iterator;
/**
 * @implements c::include::iterator::Iterator
 */
struct triangle_iterator    {
    unsigned long long current;
    unsigned long long idx;
    IteratorTail(unsigned long long, triangle_iterator)
};

/**
 * @memberof triangle_iterator
 * @private
 */
unsigned long long advance_triangle_iterator(triangle_iterator *ti) {
    IterationHead(ti);
    ti->idx++;
    ti->current += ti->idx;
    return ti->current;
}

/**
 * @memberof triangle_iterator
 */
triangle_iterator triangle_iterator0();
inline triangle_iterator triangle_iterator0()   {
    return (triangle_iterator) IteratorInitHead(advance_triangle_iterator);
}

int main(int argc, char const *argv[])  {
    triangle_iterator ti = triangle_iterator0();
    unsigned long long current;
    while (true)    {
        current = next(ti);
        // printf("%llu\n", current);
        if (proper_divisor_count(current) > 499)    {
            printf("%llu", current);
            return 0;
        }
    }
    return -1;
}

#ifdef DOXYGEN
}
#endif
