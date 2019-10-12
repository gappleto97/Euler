#ifndef ITERATOR_H
#define ITERATOR_H

#include <stdbool.h>
#include <stdint.h>
#include "macros.h"

#define IteratorTail(return_type, struct_type) \
    return_type (*iterator_function)(struct_type *it); \
    bool exhausted : 1; \
    bool started : 1; \
    bool phase : 1
    /**
     * The base struct for all iterators in this project
     * @return_type: The type that your iterator will yield
     * @struct_type: The type that your iterator state is stored in
     *
     * @iterator_function: The function to advance the iterator and return the next element
     * @exhausted: An indicator that tells you if the iterator is exhausted
     * @started: An indicator that tells you if the interator has moved at all
     * @phase: An indicator that flips every time the iterator moves
     *
     * See counter for an example implementation
     * It is recommended that you put this at the end of your struct for better packing, but if
     * you are going to use bitfield elements, it should go before that.
     */

#define IterationHead(it) \
    it->started = 1; \
    it->phase = !(it->phase)
    /**
     * The base macro for all iteration functions in this project
     * @it: The pointer to the iterator you are advancing
     *
     * See counter for an example implementation
     */

#define IteratorInitHead(it, advance) \
    it.iterator_function = &advance; \
    it.started = 0; \
    it.phase = 0; \
    it.exhausted = 0
    /**
     * The base macro for all iterator initialization functions in this project
     * @it: The iterator you are initializing
     * @advance: The function this iterator uses to advance
     *
     * See counter for an example implementation
     */

#define next(state) (*(state.iterator_function))(&state)
    /**
     * The macro to advance generic iterators
     * @state: The iterator you wish to advance
     *
     * See counter for an example implementation
     */

#define next_p(state) (*(state->iterator_function))(state)
    /**
     * The macro to advance generic iterator pointers
     * @state: The pointer to the iterator you wish to advance
     *
     * See counter for an example implementation
     */

typedef struct counter counter;
struct counter {
    /**
     * The reference struct for all iterators in this project
     * @iterator_function: The function to advance the iterator and return the next element
     * @exhausted: An indicator that tells you if the iterator is exhausted
     * @started: An indicator that tells you if the interator has moved at all
     * @phase: An indicator that flips every time the iterator moves
     * @idx: The current position of the counter
     * @stop: The point where the counter is exhausted
     * @step: The amount to step by each time you iterate the counter
     *
     * See IteratorTail
     */
    uintmax_t idx;
    uintmax_t stop;
    intmax_t step;
    IteratorTail(uintmax_t, counter);
};

uintmax_t iterate_counter(counter *i);
inline uintmax_t iterate_counter(counter *i)    {
    /**
     * The function to advance a counter
     * @i the counter you want to advance
     *
     * Returns the next number in the iteration
     */
    IterationHead(i);
    uintmax_t ret = i->idx;
    intmax_t step = i->step;
    i->idx += step;
    if ((step > 0 && i->idx >= i->stop) || (step < 0 && i->idx <= i->stop))   {
        i->exhausted = 1;
    }
    return ret;
}

counter counter3(uintmax_t start, uintmax_t stop, intmax_t step);
inline counter counter3(uintmax_t start, uintmax_t stop, intmax_t step) {
    /**
     * The base constructor for the counter iterator
     * @start: The beginning position of the counter
     * @stop: The point where the counter is exhausted
     * @step: The amount to step by each time you iterate the counter
     *
     * See counter
     */
    counter ret;
    IteratorInitHead(ret, iterate_counter);
    ret.idx = start;
    ret.stop = stop;
    ret.step = step;
    return ret;
}

counter counter2(uintmax_t start, uintmax_t stop);
inline counter counter2(uintmax_t start, uintmax_t stop)  {
    /**
     * The simpler constructor for the counter iterator
     * @start: The beginning position of the counter
     * @stop: The point where the counter is exhausted
     *
     * See counter
     */
    return counter3(start, stop, 1);
}

counter counter1(uintmax_t stop);
inline counter counter1(uintmax_t stop)    {
    /**
     * The simplest constructor for the counter iterator
     * @stop: The point where the counter is exhausted
     *
     * See counter
     */
    return counter2(0, stop);
}

#endif
