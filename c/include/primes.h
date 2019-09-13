#ifndef _PRIMES
#define _PRIMES

#include <stdlib.h>
#include <math.h>
#include "iterator.h"

typedef struct prime_counter prime_counter;
struct prime_counter {
    /**
     * The reference struct for all iterators in this project
     * @iterator_function: The function to advance the iterator and return the next element
     * @exhausted: An indicator that tells you if the iterator is exhausted
     * @started: An indicator that tells you if the interator has moved at all
     * @phase: An indicator that flips every time the iterator moves
     * @idx: The current position of the counter
     * @stop: The point where the counter is exhausted
     *
     * See IteratorHead
     */
    IteratorHead(unsigned long long, prime_counter);
    size_t idx;
    unsigned long long stop;
};

static unsigned long long *prime_cache;
// note: If you let it, this will grow indefinitely. To not let it do so, #define PRIME_CACHE_SIZE_LIMIT
static unsigned long long prime_cache_max = 0;
static size_t prime_cache_size = 0;
static size_t prime_cache_idx = 0;

unsigned long long advance_prime_counter(prime_counter *pc) {
    /**
     * The function to advance a prime number generator
     * @pc the counter you want to advance
     *
     * Returns the next number in the iteration
     */
    IterationHead(pc);
    if (!prime_cache_size)  {  // if not already done, initialize the prime cache
        prime_cache = (unsigned long long *) malloc(sizeof(unsigned long long) * 4);
        prime_cache[0] = 2;
        prime_cache[1] = 3;
        prime_cache[2] = 5;
        prime_cache[3] = prime_cache_max = 7;
        prime_cache_size = 4;
        prime_cache_idx = 4;
    }
    if (pc->idx < prime_cache_idx)  {
        unsigned long long p = prime_cache[pc->idx++];
        if (pc->exhausted = (p >= pc->stop))    {
            return 0;
        }
        return p;
    }
    for (unsigned long long p = prime_cache[pc->idx - 1] + 2; p < pc->stop; p += 2) {
        bool broken = false;
        for (size_t idx = 0; idx < prime_cache_idx; idx++)  {
            if (p % prime_cache[idx] == 0)  {  // is not prime
                broken = true;
                break;
            }
        }
        if (!broken)    {  // primeness not determined, exceeded cache
            unsigned long long root_p = ceil(sqrt(p));
            for (unsigned long long c = prime_cache_max; c <= root_p; c += 2)  {
                if (p % c == 0) {  // is not prime
                    broken = true;
                    break;
                }
            }
        }
        if (!broken)    {  // is prime
            if (pc->idx == prime_cache_idx) {
#ifdef PRIME_CACHE_SIZE_LIMIT
                if (prime_cache_size == prime_cache_idx && prime_cache_size < PRIME_CACHE_SIZE_LIMIT)   {
#else
                if (prime_cache_size == prime_cache_idx)    {
#endif
                    size_t new_size = prime_cache_size * 2;
#ifdef PRIME_CACHE_SIZE_LIMIT
                    if (new_size > PRIME_CACHE_SIZE_LIMIT)  {
                        new_size = PRIME_CACHE_SIZE_LIMIT;
                    }
#endif
                    void *tmp = realloc(prime_cache, new_size * sizeof(unsigned long long));
                    if (tmp != NULL)    {
                        prime_cache = (unsigned long long *) tmp;
                        prime_cache_size = new_size;
                        prime_cache[prime_cache_idx++] = prime_cache_max = p;
                    }
                } else  {
                    prime_cache[prime_cache_idx++] = p;
                }
            }
            pc->idx++;
            if (pc->exhausted = (p >= pc->stop))    {
                return 0;
            }
            return p;
        }
    }
    pc->exhausted = true;  // shouldn't get here, but just in case
    return 0;
}

prime_counter prime_counter1(unsigned long long stop)  {
    /**
     * The base constructor for the prime number generator
     * @stop: The point where the counter is exhausted
     *
     * See prime_counter
     */
    prime_counter ret;
    IteratorInitHead(ret, advance_prime_counter);
    ret.idx = 0;
    ret.stop = stop;
    return ret;
}

prime_counter prime_counter0()  {
    /**
     * The simplest constructor for the prime number generator
     *
     * See prime_counter
     */
    return prime_counter1(-1);
}

typedef struct prime_factor_counter prime_factor_counter;
struct prime_factor_counter {
    /**
     * The iterator that allows you to prime factorize a number
     * @iterator_function: The function to advance the iterator and return the next element
     * @exhausted: An indicator that tells you if the iterator is exhausted
     * @started: An indicator that tells you if the interator has moved at all
     * @phase: An indicator that flips every time the iterator moves
     * @target: The current target for prime factorization (note: this will change after construction)
     * @current: The prime number most recently tested
     * @pc: The prime number generator being used to test
     *
     * See IteratorHead
     */
    IteratorHead(unsigned long long, prime_factor_counter);
    unsigned long long target;
    unsigned long long current;
    prime_counter pc;
};

unsigned long long advance_prime_factor_counter(prime_factor_counter *pfc)  {
    /**
     * The function to advance a prime factor iterator
     * @i the counter you want to advance
     *
     * Returns the next number in the iteration
     */
    while (pfc->target != 0 && pfc->target != 1 && !pfc->pc.exhausted) {
        pfc->current = next(pfc->pc);
        if (pfc->target % pfc->current == 0)    {
            pfc->target /= pfc->current;
            pfc->exhausted = (pfc->target == 1);
            return pfc->current;
        }
    }
    pfc->exhausted = true;
    return -1;
}

prime_factor_counter prime_factors(unsigned long long n)    {
    /**
     * The base constructor for the prime factors iterator
     * @n: The non-zero number you wish to factor
     *
     * WARNING: if you put in 0, behaviour is undefined
     *
     * See prime_factor_counter
     */
    prime_factor_counter ret;
    IteratorInitHead(ret, advance_prime_factor_counter);
    ret.current = 0;
    ret.target = n;
    ret.pc = prime_counter0();
    return ret;
}

unsigned long long is_composite(unsigned long long n)   {
    /**
     * Tells you if a number is composite, and if so, its smallest prime factor
     * @n: The number you wish to test
     *
     * See prime_factor_counter
     */
    if (!n || n == 1)   {
        return 0;
    }
    prime_factor_counter iter = prime_factors(n);
    return next(iter);
}

bool is_prime(unsigned long long n) {
    /**
     * Tells you if a number is prime
     * @n: The number you wish to test
     *
     * See prime_factor_counter
     */
    return n && n != 1 && !is_composite(n);
}

#endif
