#ifndef DIVISORS_H
#define DIVISORS_H

#include "iterator.h"

typedef struct factor_counter factor_counter;
struct factor_counter   {
    uintmax_t target;
    uintmax_t current;
    IteratorTail(uintmax_t, factor_counter);
};

uintmax_t advance_factor_counter(factor_counter *fc);
inline uintmax_t advance_factor_counter(factor_counter *fc) {
    IterationHead(fc);
    while (fc->target > fc->current)   {
        fc->current++;
        if (fc->target % fc->current == 0)  {
            fc->exhausted = (fc->target == fc->current);
            return fc->current;
        }
    }
    return 0;
}

factor_counter proper_divisors(uintmax_t target);
inline factor_counter proper_divisors(uintmax_t target) {
    factor_counter ret;
    IteratorInitHead(ret, advance_factor_counter);
    ret.target = target;
    ret.current = 0;
    return ret;
}

uintmax_t proper_divisor_count(uintmax_t target);
inline uintmax_t proper_divisor_count(uintmax_t target) {
    uintmax_t ret = 0;
    factor_counter fc = proper_divisors(target);
    while (!fc.exhausted)   {
        next(fc);
        ret++;
    }
    return ret;
}

#endif
