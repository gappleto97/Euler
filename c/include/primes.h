#ifndef PRIMES_H
#define PRIMES_H

#include <stdint.h>
#include "macros.h"

#if !PCC_COMPILER
    #include <stdlib.h>
    #include <math.h>
#else
    #include "math.h"
#endif

#include "iterator.h"

typedef struct prime_sieve prime_sieve;
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
    IteratorHead(uintmax_t, prime_counter);
    size_t idx;
    uintmax_t stop;
    prime_sieve *ps;
};

static uintmax_t *prime_cache;
// note: If you let it, this will grow indefinitely. To not let it do so, #define PRIME_CACHE_SIZE_LIMIT
static uintmax_t prime_cache_max = 0;
static size_t prime_cache_size = 0;
static size_t prime_cache_idx = 0;
prime_sieve prime_sieve0();

uintmax_t advance_prime_counter(prime_counter *pc) {
    /**
     * The function to advance a prime number generator
     * @pc the counter you want to advance
     *
     * Returns the next number in the iteration
     */
    IterationHead(pc);
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
    for (uintmax_t p = prime_cache[pc->idx - 1] + 2; p < pc->stop; p += 2) {
        bool broken = false;
        for (size_t idx = 1; idx < prime_cache_idx; idx++)  {
            if (p % prime_cache[idx] == 0)  {  // is not prime
                broken = true;
                break;
            }
        }
        if (!broken)    {  // primeness not determined, exceeded cache
            uintmax_t root_p = ceil(sqrt(p));
            for (uintmax_t c = prime_cache_max; c <= root_p; c += 2)  {
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
                    uintmax_t *tmp = (uintmax_t *) realloc(prime_cache, new_size * sizeof(uintmax_t));
                    if (tmp != NULL)    {
                        prime_cache = tmp;
                        prime_cache_size = new_size;
                        prime_cache[prime_cache_idx++] = prime_cache_max = p;
                    }
                } else  {
                    prime_cache[prime_cache_idx++] = p;
                }
            }
            pc->idx++;
            if ((pc->exhausted = (p >= pc->stop)))  {
                return 0;
            }
            return p;
        }
    }
    pc->exhausted = true;  // shouldn't get here, but just in case
    return 0;
}

prime_counter prime_counter1(uintmax_t stop)  {
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
    ret.ps = NULL;
    return ret;
}

prime_counter prime_counter0();
inline prime_counter prime_counter0()   {
    /**
     * The simplest constructor for the prime number generator
     *
     * See prime_counter
     */
    return prime_counter1(-1);
}

struct prime_sieve {
    /**
     * The iterator that implements a modified sieve of eratosthenes
     * @iterator_function: The function to advance the iterator and return the next element
     * @exhausted: An indicator that tells you if the iterator is exhausted
     * @started: An indicator that tells you if the interator has moved at all
     * @phase: An indicator that flips every time the iterator moves
     * @sieve: The sieve state used to generate new primes
     * @sieve_len: The length of the sieve state (divided by 2)
     * @prime: The current reference prime
     * @prime_squared: The reference prime squared
     * @candidate: The current candidate prime number
     * @source: The source of new reference prime numbers
     *
     * See IteratorHead
     */
    IteratorHead(uintmax_t, prime_sieve);
    uintmax_t *sieve;
    size_t sieve_len;
    uintmax_t prime;
    uintmax_t prime_squared;
    uintmax_t candidate;
    prime_counter source;
};

uintmax_t advance_prime_sieve(prime_sieve *ps) {
    /**
     * The function to advance a prime sieve iterator
     * @ps the sieve you want to advance
     *
     * Returns the next prime number in the iteration
     */
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

prime_sieve prime_sieve0()  {
    /**
     * The constructor for the prime number sieve
     *
     * See prime_sieve
     */
    prime_sieve ret;
    IteratorInitHead(ret, advance_prime_sieve);
    ret.sieve = NULL;
    ret.sieve_len = 0;
    ret.prime = 3;
    ret.prime_squared = 9;
    ret.candidate = 2;
    ret.source = prime_counter0();
    next(ret.source);
    next(ret.source);
    return ret;
}

void free_prime_counter(prime_counter pc);
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
    IteratorHead(uintmax_t, prime_factor_counter);
    uintmax_t target;
    uintmax_t current;
    prime_counter pc;
};

uintmax_t advance_prime_factor_counter(prime_factor_counter *pfc)  {
    /**
     * The function to advance a prime factor iterator
     * @i the counter you want to advance
     *
     * Returns the next number in the iteration
     */
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

prime_factor_counter prime_factors(uintmax_t n)    {
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
    ret.current = 2;
    ret.target = n;
    ret.pc = prime_counter0();
    next(ret.pc);
    return ret;
}

#define free_prime_factor_counter(pfc) free_prime_counter(pfc.pc)

uintmax_t is_composite(uintmax_t n)   {
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
    uintmax_t ret = next(iter);
    free_prime_factor_counter(iter);
    if (ret == n)   {
        return 0;
    }
    return ret;
}

bool is_prime(uintmax_t n);
inline bool is_prime(uintmax_t n)  {
    /**
     * Tells you if a number is prime
     * @n: The number you wish to test
     *
     * See prime_factor_counter
     */
    return n && n != 1 && !is_composite(n);
}

#endif
