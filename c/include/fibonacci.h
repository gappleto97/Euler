#ifndef FIBONACCI_H
#define FIBONACCI_H

#include "iterator.h"

typedef struct fibonacci fibonacci;
struct fibonacci    {
    uintmax_t a;
    uintmax_t b;
    uintmax_t limit;
    IteratorTail(uintmax_t, fibonacci);
};

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

fibonacci fibonacci1(uintmax_t limit)  {
    fibonacci ret;
    IteratorInitHead(ret, advance_fibonacci);
    ret.a = 0;
    ret.b = 1;
    ret.limit = limit;
    return ret;
}

fibonacci fibonacci0();
inline fibonacci fibonacci0()   {
    return fibonacci1(-1);
}

#endif
