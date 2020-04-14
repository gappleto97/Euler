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
fibonacci fibonacci1(uintmax_t limit);
inline fibonacci fibonacci1(uintmax_t limit)    {
    return (fibonacci) IteratorInitHead(advance_fibonacci, ExtendInit(b, 1), ExtendInit(limit, limit));
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
