#ifndef PRIMES_H
#define PRIMES_H

#include <stdint.h>
#include <string.h>
#include "macros.h"

#if !PCC_COMPILER
    #include <stdlib.h>
    #include <math.h>
#else
    #include "./math.h"
#endif

#include "iterator.h"

#ifdef DOXYGEN
namespace c::include::primes {
#endif

typedef struct prime_sieve prime_sieve;

typedef struct prime_counter prime_counter;
/**
 * @implements c::include::iterator::Iterator
 * @brief A cached prime number generator
 * @note
 * If you will only use the prime numbers once in your program, use @ref prime_sieve instead
 */
struct prime_counter {
    size_t idx;  /**< The current position of the counter */
    uintmax_t stop;  /**< The point where the counter is exhausted */
    prime_sieve *ps;  /**< A (currently unused) source for new prime numbers */
    IteratorTail(uintmax_t, prime_counter)
};

/**
 * @implements c::include::iterator::Iterator
 * The iterator that implements a modified sieve of eratosthenes
 */
struct prime_sieve {
    uintmax_t *sieve;  /**< The sieve state used to generate new primes stored as pairs of [value, step] */
    size_t sieve_len;  /**< The length of the sieve state (divided by 2) */
    uintmax_t prime;  /**< The current reference prime */
    uintmax_t prime_squared;  /**< The reference prime squared */
    uintmax_t candidate;  /**< The current candidate prime number */
    prime_counter source;  /**< The source of new reference prime numbers */
    IteratorTail(uintmax_t, prime_sieve)
};

static uintmax_t *prime_cache;
// note: If you let it, this will grow indefinitely. To not let it do so, #define PRIME_CACHE_SIZE_LIMIT
static uintmax_t prime_cache_max = 0;
static size_t prime_cache_size = 0;
static size_t prime_cache_idx = 0;
prime_sieve prime_sieve0();

/**
 * The function to advance a prime number generator
 * @param pc the counter you want to advance
 * @memberof prime_counter
 * @private
 *
 * @returns the next number in the iteration
 */
uintmax_t advance_prime_counter(prime_counter *pc) {
    IterationHead(pc);
    if (unlikely(pc->exhausted))    {
        return 0;
    }
    if (!prime_cache_size)  {  // if not already done, initialize the prime cache
        prime_cache = (uintmax_t *) malloc(sizeof(uintmax_t) * 4);
        prime_cache[0] = 2;
        prime_cache[1] = 3;
        prime_cache[2] = 5;
        prime_cache[3] = prime_cache_max = 7;
        prime_cache_size = 4;
        prime_cache_idx = 4;
    }
    if (pc->idx < prime_cache_idx)  {
        uintmax_t p = prime_cache[pc->idx++];
        if ((pc->exhausted = (p >= pc->stop)))  {
            return 0;
        }
        return p;
    }
    if (pc->ps == NULL) {
        // keep in mind that recursion on this block will never continue beyond 1 level unless the nested pc exceeds the cache size.
        // pc makes a ps. ps makes a nested pc. the nested pc never makes a ps because it can feed entirely on the cache
        // similar story if you start from ps. ps makes a pc. pc eventually makes a nested ps, which makes a nested pc
        // the nested pc feeds entirely off the cache and never makes a doubly-tested pc.
        prime_sieve to_copy = prime_sieve0();
        char *ptr = (char *) malloc(sizeof(prime_sieve));
        memcpy(ptr, &to_copy, sizeof(prime_sieve));
        pc->ps = (prime_sieve *) ptr;
        while(next_p(pc->ps) < prime_cache[pc->idx - 1]) {}
    }
    const uintmax_t p = prime_cache[pc->idx] = next_p(pc->ps);
    prime_cache_max = max(prime_cache_max, prime_cache[pc->idx]);
    prime_cache_idx = max(prime_cache_idx, pc->idx);
    if (pc->idx == prime_cache_idx) {
        if (
            prime_cache_size == prime_cache_idx
            #ifdef PRIME_CACHE_SIZE_LIMIT
                && prime_cache_size < PRIME_CACHE_SIZE_LIMIT
            #endif
        )   {
            size_t new_size = prime_cache_size * 2;
            #ifdef PRIME_CACHE_SIZE_LIMIT
                if (new_size > PRIME_CACHE_SIZE_LIMIT)  {
                    new_size = PRIME_CACHE_SIZE_LIMIT;
                }
            #endif
            uintmax_t *tmp = (uintmax_t *) realloc(prime_cache, new_size * sizeof(uintmax_t));
            if (tmp != NULL)    {
                prime_cache = tmp;
                prime_cache_size = new_size;
                prime_cache[prime_cache_idx++] = prime_cache_max = p;
            }
        }
        else    {
            prime_cache[prime_cache_idx++] = p;
        }
    }
    pc->idx++;
    if ((pc->exhausted = (p >= pc->stop)))  {
        return 0;
    }
    return p;
}

/**
 * The base constructor for the prime number generator
 * @param stop The point where the counter is exhausted
 *
 * @memberof prime_counter
 */
prime_counter prime_counter1(uintmax_t stop);
inline prime_counter prime_counter1(uintmax_t stop) {
    return (prime_counter) IteratorInitHead(advance_prime_counter, ExtendInit(stop, stop));
}

/**
 * The simplest constructor for the prime number generator
 *
 * @memberof prime_counter
 */
prime_counter prime_counter0();
inline prime_counter prime_counter0()   {
    return prime_counter1(-1);
}

/**
 * The function to advance a prime sieve iterator
 * @param ps the sieve you want to advance
 * @memberof prime_sieve
 * @private
 *
 * @returns the next prime number in the iteration
 */
uintmax_t advance_prime_sieve(prime_sieve *ps) {
    // modified sieve of eratosthenes adapted to C from Python p0003
    if (ps->candidate == 2) {
        ps->candidate = 3;
        return 2;
    }
    // if candidate in sieve
    while (true)    {
        uintmax_t step;
        bool candidate_in_sieve = false;
        size_t candidate_index = -1;
        for (size_t i = 0; i < ps->sieve_len * 2; i += 2)   {
            if (ps->sieve[i] == ps->candidate)  {
                step = ps->sieve[i + 1];
                candidate_in_sieve = true;
                candidate_index = i;
                break;
            }
        }
        if (!candidate_in_sieve)    {
            if (ps->candidate < ps->prime_squared)  {  // prime
                uintmax_t ret = ps->candidate;
                ps->candidate += 2;
                return ret;
            }
            // == prime_squared
            step = ps->prime * 2;
            ps->prime = next(ps->source);
            ps->prime_squared = ps->prime * ps->prime;
        }
        uintmax_t candidate = ps->candidate;
        ps->candidate += 2;
        do {
            candidate += step;
            candidate_in_sieve = false;
            for (size_t i = 0; i < ps->sieve_len * 2; i += 2)   {
                if (ps->sieve[i] == candidate)  {
                    candidate_in_sieve = true;
                    break;
                }
            }
        } while (candidate_in_sieve);
        if (candidate_index != -1)  {
            ps->sieve[candidate_index] = candidate;
        } else  {
            ps->sieve_len++;
            ps->sieve = (uintmax_t *) realloc(ps->sieve, ps->sieve_len * 2 * sizeof(uintmax_t));
            ps->sieve[(ps->sieve_len - 1) * 2] = candidate;
            ps->sieve[ps->sieve_len * 2 - 1] = step;
        }
    }
}

/**
 * The constructor for the prime number sieve
 *
 * @memberof prime_sieve
 */
prime_sieve prime_sieve0()  {
    prime_sieve ret = IteratorInitHead(
        advance_prime_sieve,
        ExtendInit(source, prime_counter0()),
        ExtendInit(prime, 3),
        ExtendInit(prime_squared, 9),
        ExtendInit(candidate, 2)
    );
    next(ret.source);
    next(ret.source);
    return ret;
}

/**
 * The destructor for the prime number counter
 *
 * @memberof prime_counter
 */
void free_prime_counter(prime_counter pc);

/**
 * The destructor for the prime number sieve
 *
 * @memberof prime_sieve
 */
void free_prime_sieve(prime_sieve ps)   {
    free_prime_counter(ps.source);
    if (ps.sieve != NULL)   {
        free(ps.sieve);
    }
}

void free_prime_counter(prime_counter pc)   {
    if (pc.ps != NULL)  {
        free_prime_sieve(*pc.ps);
        free(pc.ps);
    }
}


typedef struct prime_factor_counter prime_factor_counter;
/**
 * @implements c::include::iterator::Iterator
 * The iterator that allows you to prime factorize a number
 */
struct prime_factor_counter {
    uintmax_t target;  /**< The current target for prime factorization @note this will change after construction */
    uintmax_t current;  /**< The prime number most recently tested */
    prime_counter pc;  /**< The prime number generator being used to test */
    IteratorTail(uintmax_t, prime_factor_counter)
};

/**
 * The function to advance a prime factor iterator
 * @param i the counter you want to advance
 * @memberof prime_factor_counter
 * @private
 *
 * @returns the next prime factor OR @c -1
 */
uintmax_t advance_prime_factor_counter(prime_factor_counter *pfc)  {
    while (pfc->target != 0 && pfc->target != 1 && !pfc->pc.exhausted) {
        if (pfc->target % pfc->current == 0)    {
            pfc->target /= pfc->current;
            pfc->exhausted = (pfc->target == 1);
            return pfc->current;
        }
        pfc->current = next(pfc->pc);
    }
    pfc->exhausted = true;
    return -1;
}

/**
 * The base constructor for the prime factors iterator
 * @param n The non-zero number you wish to factor
 * @memberof prime_factor_counter
 * @private
 * @attention
 * If you put in 0, behaviour is undefined
 */
prime_factor_counter prime_factors(uintmax_t n)    {
    prime_factor_counter ret = IteratorInitHead(
        advance_prime_factor_counter,
        ExtendInit(pc, prime_counter0()),
        ExtendInit(current, 2),
        ExtendInit(target, n)
    );
    next(ret.pc);
    return ret;
}

/**
 * The destructor for the prime factor generator
 *
 * @memberof prime_factor_counter
 */
void free_prime_factor_counter(prime_factor_counter pfc);
inline void free_prime_factor_counter(prime_factor_counter pfc) {
    free_prime_counter(pfc.pc);
}

/**
 * Tells you if a number is composite, and if so, its smallest prime factor
 * @param n The number you wish to test
 * @returns Either 0 or the number's smallest prime factor
 *
 * @relatesalso prime_factor_counter
 */
uintmax_t is_composite(uintmax_t n)   {
    if (!n || n == 1)   {
        return 0;
    }
    prime_factor_counter iter = prime_factors(n);
    uintmax_t ret = next(iter);
    free_prime_factor_counter(iter);
    if (ret == n)   {
        return 0;
    }
    return ret;
}

/**
 * Tells you if a number is prime
 * @param n The number you wish to test
 * @returns A bool indicating whether a number is prime
 *
 * @relatesalso prime_factor_counter
 */
bool is_prime(uintmax_t n);
inline bool is_prime(uintmax_t n)  {
    return n && n != 1 && !is_composite(n);
}

#ifdef DOXYGEN
};
#endif

#endif
