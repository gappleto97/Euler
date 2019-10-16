#ifndef FIBONACCI_H
#define FIBONACCI_H

#include "iterator.h"

#ifdef DOXYGEN
namespace c::include::fibonacci {
#endif


typedef struct fibonacci fibonacci;
/**
 * @implements c::include::iterator::Iterator
 */
struct fibonacci    {
    uintmax_t a;
    uintmax_t b;
    uintmax_t limit;
    IteratorTail(uintmax_t, fibonacci)
};

/**
 * @memberof fibonacci
 * @private
 */
uintmax_t advance_fibonacci(fibonacci *fib)    {
    if (fib->exhausted) {
        return 0;
    }
    IterationHead(fib);
    uintmax_t tmp = fib->a + fib->b;
    fib->a = fib->b;
    fib->b = tmp;
    fib->exhausted = (tmp > fib->limit);
    return fib->a;
}

/**
 * @memberof fibonacci
 */
fibonacci fibonacci1(uintmax_t limit)  {
    fibonacci ret = IteratorInitHead(advance_fibonacci);
    ret.a = 0;
    ret.b = 1;
    ret.limit = limit;
    return ret;
}

/**
 * @memberof fibonacci
 */
fibonacci fibonacci0();
inline fibonacci fibonacci0()   {
    return fibonacci1(-1);
}

#ifdef DOXYGEN
};
#endif

#endif
