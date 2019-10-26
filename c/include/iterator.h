/**
 * @file iterator.h
 * @author Gabe Appleton
 * @date 15 Oct 2019
 * @brief An implementation of Python-like iterators and generators using macros to maintain static typing
 */
#ifndef ITERATOR_H
#define ITERATOR_H

#include <stdbool.h>
#include <stdint.h>
#include "macros.h"

#ifdef DOXYGEN
namespace c::include::iterator {
#endif

void no_destructor(void *it);
inline void no_destructor(void *it) {}

/**
 * @brief The base definition macro for all iterators in this project
 * @param return_type The type that your iterator will yield
 * @param struct_type The type that your iterator state is stored in
 * @relatedalso c::include::iterator::Iterator
 * @see counter
 * for an example implementation
 * @remark
 * It is recommended that you put this at the end of your struct for better packing, but if
 * you are going to use bitfield elements, it should go before that.
 * @attention
 * When you are declaring an Iterator struct, you MUST DECLARE IT FIRST in order to feed it to this macro
 *
 * Example:
 * @code{.c}
 * typedef struct counter counter;
 * struct counter {
 *     uintmax_t idx;
 *     uintmax_t stop;
 *     intmax_t step;
 *     IteratorTail(uintmax_t, counter)
 *     bool example_bit : 1;
 * };
 * @endcode
 */
#define IteratorTail(return_type, struct_type) \
    return_type (*const iterator_function)(struct_type *it); /**< @private The pointer to the iteration function @see next */\
    void (*const destructor)(void *it); /**< @private The pointer to the destructor function @see free_iterator @cond EXPAND_ITERATOR_TAIL */\
    bool exhausted : 1;\
    bool started : 1;\
    bool phase : 1; /** @endcond */

/**
 * @brief The base macro for all iteration functions in this project
 * @param it The pointer to the iterator you are advancing
 * @relatedalso c::include::iterator::Iterator
 * @see counter
 * for an example implementation
 *
 * Example:
 * @code{.c}
 * static uintmax_t iterate_counter(counter *ct) {
 *     IterationHead(ct);
 *     if (!ct->exhausted) {
 *         const uintmax_t ret = ct->idx;
 *         ct->idx += ct->step;
 *         ct->exhausted = (ct->idx >= ct->stop);
 *         return ret;
 *     }
 *     return -1;  // error
 * }
 * @endcode
 */
#define IterationHead(it) \
    it->started = 1; \
    it->phase = !(it->phase)

/**
 * @brief The base macro for all iterator initialization functions in this project
 * @param advance The function this iterator uses to advance
 * @param ... Additional things to initialize wrapped in ExtendInit()
 * @returns A compound literal that initializes the @ref c::include::iterator::Iterator "Iterator-defined" portions of your subclass
 * @relatedalso c::include::iterator::Iterator
 * @see counter
 * for an example implementation
 * @note
 * All values not directly related to the @ref c::include::iterator::Iterator "Iterator" class will be initialized according to the C99 rules
 * for compound literals. This means:
 * @li Numeric types are initialized to @c 0
 * @li Pointer types are initialized to @c NULL
 * @li Composite types are initialized recursively by these rules
 *
 * @attention
 * The iterator function you give it MUST take in ONLY a pointer to the declared struct
 *
 * @attention
 * Because the pointer to the iteration function is constant, any nested @ref c::include::iterator::Iterator "Iterators" must be initialized
 * in the original assigning expression, and @ref c::include::iterator::Iterator "Iterators" cannot be reassigned to entierely. It is often
 * recommended to provide a "reset" function as an alternative to the standard constructor if you want these objects to be reused.
 *
 * Example:
 * @code{.c}
 * counter counter0() {
 *     counter ct = IteratorInitHead(iterate_counter);
 *     ct.step = 1;
 *     ct.stop = 100;
 *     return ct;  // note all other values are initialized to 0
 * }
 * @endcode
 */
#define IteratorInitHead(advance, ...) {.iterator_function = &advance, AddDestructor(no_destructor), __VA_ARGS__}

/**
 * @brief The extension macro for initializing @ref c::include::iterator::Iterator "Iterators" with a destructor
 * @param func The destructor function of your iterator
 * @relatedalso c::include::iterator::Iterator
 *
 * @attention
 * When adding a destructor to an @ref c::include::iterator::Iterator "Iterator" subclass it MUST have the
 * following signature:
 * @code{.c}
 * void function_name(iterator_subclass *is)
 * @endcode
 * Note, however, that it will be cast to
 * @code{.c}
 * void (*)(void *)
 * @endcode
 * for compatibility with the default destructor. This means that you may not see a type error if you enter it incorrectly.
 *
 * Assignment example:
 * @code{.c}
 * counter counter0() {
 *     counter ct = IteratorInitHead(iterate_counter, AddDestructor(free_counter));
 *     return ct;  // note all other values are initialized to 0
 * }
 * @endcode
 *
 * Direct return example:
 * @code{.c}
 * counter counter0() {
 *     return (counter) IteratorInitHead(iterate_counter, AddDestructor(free_counter));
 * }
 * @endcode
 */
#define AddDestructor(func) ExtendInit(destructor, (void (*const)(void *)) &func)

/**
 * @brief The extension macro for initializing more complicated @ref c::include::iterator::Iterator "Iterators"
 * @param name The name you would like to assign to in the struct (do not include the struct name or initial .)
 * @param value The value you would like to assign
 * @relatedalso c::include::iterator::Iterator
 *
 * Assignment example:
 * @code{.c}
 * counter counter0() {
 *     counter ct = IteratorInitHead(iterate_counter, ExtendInit(step, 1), ExtendInit(stop, 100));
 *     return ct;  // note all other values are initialized to 0
 * }
 * @endcode
 *
 * Direct return example:
 * @code{.c}
 * counter counter0() {
 *     return (counter) IteratorInitHead(iterate_counter, ExtendInit(step, 1), ExtendInit(stop, 100));
 * }
 * @endcode
 */
#define ExtendInit(name, value) .name = value

/**
 * @brief The macro to advance generic iterators
 * @param state The iterator you wish to advance
 * @returns The next item generated by the given iterator
 * @relatedalso c::include::iterator::Iterator
 * @see next_p
 * For a version that deals with pointers
 * @see counter
 * for an example implementation
 */
#define next(state) (*(state.iterator_function))(&state)

/**
 * @brief The macro to advance generic iterator pointers
 * @param state The pointer to the iterator you wish to advance
 * @returns The next item generated by the given iterator
 * @relatedalso c::include::iterator::Iterator
 * @see next
 * For a version that deals with direct objects
 * @see counter
 * for an example implementation
 */
#define next_p(state) (*(state->iterator_function))(state)

/**
 * @brief The generic destructor for iterators
 * @param it The iterator you wish to destruct
 * @relatedalso c::include::iterator::Iterator
 * @see free_iterator_p
 * For a version that deals with pointers
 * @see counter
 * for an example implementation
 * @note
 * Any given subclass might not include a destructor if nothing needs cleaning.
 * That does not mean you should not call this function, as that may change in the future,
 * and on most compilers it typically adds very little overhead (order of 3 instructions).
 */
#define free_iterator(it) (*(it.destructor))(&it)

/**
 * @brief The generic destructor for iterator pointers
 * @param it A pointer to the iterator you wish to destruct
 * @relatedalso c::include::iterator::Iterator
 * @see free_iterator
 * For a version that deals with direct objects
 * @see counter
 * for an example implementation
 * @note
 * Any given subclass might not include a destructor if nothing needs cleaning.
 * That does not mean you should not call this function, as that may change in the future,
 * and on most compilers it typically adds very little overhead (order of 3 instructions).
 */
#define free_iterator_p(it) (*(it->destructor))(it)

typedef struct Iterator Iterator;
/**
 * @brief An implementation of Python-like iterators and generators in C
 * @see counter
 * For an example implementation
 */
struct Iterator {
    bool exhausted : 1; /**< An indicator that the iterator has stopped */
    bool started : 1; /**< An indicator that the iterator has started */
    bool phase : 1; /**< An indicator that changes each time the iterator moves */
};
// note that this exists purely for documentation purposes. The tail macro has these features
// marked to be hidden so that these attributes properly appear as inherited.

typedef struct counter counter;
/**
 * @brief The reference struct for all iterators in this project
 * @implements Iterator
 */
struct counter {
    uintmax_t idx; /**< The current position of the counter */
    uintmax_t stop; /**< The value before which the counter will stop */
    intmax_t step; /**< The amount by which the counter will move */
    IteratorTail(uintmax_t, counter)
};

/**
 * @private
 * @brief The function to advance a counter
 * @param i the counter you want to advance
 * @memberof counter
 *
 * @returns the next number in the iteration
 */
static uintmax_t iterate_counter(counter *i);
static inline uintmax_t iterate_counter(counter *i) {
    IterationHead(i);
    uintmax_t ret = i->idx;
    intmax_t step = i->step;
    i->idx += step;
    if ((step > 0 && i->idx >= i->stop) || (step < 0 && i->idx <= i->stop))   {
        i->exhausted = 1;
    }
    return ret;
}

/**
 * @brief The base constructor for the counter iterator
 * @param start The beginning position of the counter
 * @param stop The point where the counter is exhausted
 * @param step The amount to step by each time you iterate the counter
 * @memberof counter
 *
 * @returns A @ref counter
 */
counter count_by(uintmax_t start, uintmax_t stop, intmax_t step);
inline counter count_by(uintmax_t start, uintmax_t stop, intmax_t step) {
    return (counter) IteratorInitHead(
        iterate_counter,
        ExtendInit(idx, start),
        ExtendInit(stop, stop),
        ExtendInit(step, step)
    );
}

/**
 * @brief The simpler constructor for the counter iterator
 * @param start The beginning position of the counter
 * @param stop The point where the counter is exhausted
 * @memberof counter
 *
 * @returns A @ref counter
 */
counter count_in_range(uintmax_t start, uintmax_t stop);
inline counter count_in_range(uintmax_t start, uintmax_t stop)  {
    return count_by(start, stop, 1);
}

/**
 * @brief The simplest constructor for the counter iterator
 * @param stop The point where the counter is exhausted
 * @memberof counter
 *
 * @returns A @ref counter
 */
counter count_to(uintmax_t stop);
inline counter count_to(uintmax_t stop)    {
    return count_in_range(0, stop);
}

#ifdef DOXYGEN
};
#endif

#endif
