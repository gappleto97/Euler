#ifndef MACROS_H
#define MACROS_H

// compiler info section

/**
 * @brief Indicates whether the code is being compiled under the Visual Studios compiler
 * This is guaranteed to be either 0 or 1
 */
#if (defined(_MSC_VER) && !defined(__clang__))
    #define CL_COMPILER 1
#else
    #define CL_COMPILER 0
#endif

/**
 * @brief Indicates whether the code is being compiled under the clang compiler
 * This is guaranteed to be either 0 or 1
 */
#if (defined(__clang__) && (!defined(AMD_COMPILER) || !AMD_COMPILER))
    #define CLANG_COMPILER 1
#else
    #define CLANG_COMPILER 0
#endif

/**
 * @brief Indicates whether the code is being compiled under the GCC compiler
 * This is guaranteed to be either 0 or 1
 */
#if (defined(__GNUC__) && !defined(__clang__)) && !defined(__INTEL_COMPILER) && !defined(__PCC__)
    #define GCC_COMPILER 1
#else
    #define GCC_COMPILER 0
#endif

/**
 * @brief Indicates whether the code is being compiled under the Intel optimized compiler
 * This is guaranteed to be either 0 or 1
 */
#ifdef __INTEL_COMPILER
    #define INTEL_COMPILER 1
#else
    #define INTEL_COMPILER 0
#endif

/**
 * @brief Indicates whether the code is being compiled under the AMD optimized compiler
 * @attention
 * The macro suite can't actually detect the difference between clang and aocc, so the user needs to specify
 *
 * This is guaranteed to be 0 if you don't define it explicitly
 */
#ifndef AMD_COMPILER
    #if CLANG_COMPILER
        #warning "This suite can't detect the difference between clang and aocc. You need to specify -DAMD_COMPILER={0 or 1}"
    #endif
    #define AMD_COMPILER 0
#endif

/**
 * @brief Indicates whether the code is being compiled under the Portable C Compiler
 * This is guaranteed to be either 0 or 1
 */
#ifdef __PCC__
    #define PCC_COMPILER 1
#else
    #define PCC_COMPILER 0
#endif

/**
 * @brief Indicates whether the code is being compiled under the Tiny C Compiler
 * This is guaranteed to be either 0 or 1 if your compiler supports constant folding, or to evaluate to 0 or 1 if not
 */
#define TCC_COMPILER (!(AMD_COMPILER || CL_COMPILER || CLANG_COMPILER || GCC_COMPILER || INTEL_COMPILER || PCC_COMPILER))

/**
 * @brief Indicates whether the code is being compiled for an amd64 or x86_64 machine
 * This is guaranteed to be either 0 or 1
 */
#if (defined(_M_X64) || defined(_M_AMD64) || defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64))
    #define X64_COMPILER 1
#else
    #define X64_COMPILER 0
#endif

/**
 * @brief Indicates whether the code is being compiled for an x86 machine
 * @note
 * If being compiled for amd64/x86_64 this will be falsey
 *
 * This is guaranteed to be either 0 or 1
 */
#if (!X64_COMPILER && (defined(_M_X86) || defined(_M_IX86) || defined(i386) || defined(__i386) || defined(__i386__) || defined(_X86_)))
    #define X86_COMPILER 1
#else
    #define X86_COMPILER 0
#endif

/**
 * @brief Indicates whether the code is being compiled for an ARM machine (regardless of 32 or 64 bit)
 * This is guaranteed to be either 0 or 1
 */
#if (defined(__arm__) || defined(__aarch64__) || defined(__thumb__) || defined(_M_ARM) || defined(_M_ARMT) || defined(__ARM_ARCH))
    #define ARM_COMPILER 1
#else
    #define ARM_COMPILER 0
#endif

/**
 * @brief Indicates whether the code is being compiled for ARM thumb mode
 * This is guaranteed to be either 0 or 1
 */
#if (ARM_COMPILER && (defined(__thumb__) || defined(_M_ARMT)))
    #define ARM_THUMB 1
#else
    #define ARM_THUMB 0
#endif

// compiler workaround section

#if PCC_COMPILER
    #ifndef NO_USER_WARNINGS
        #warning static is being redefined to '' because you are on PCC. \
        This is happening because PCC does not allow reproducible builds with the static keyword used globally. \
        Make sure this does not have side effects, or undefine/redefine static per-usage.
    #endif
    #define static
#endif

// helper macro function section

/**
 * @brief Given two comparable variables, return the greater
 * @param a An object to compare
 * @param b An object to compare
 * @returns The greater of a and b
 */
#ifndef max
    #define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

/**
 * @brief Given two comparable variables, return the lesser
 * @param a An object to compare
 * @param b An object to compare
 * @returns The lesser of a and b
 */
#ifndef min
    #define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

/**
 * @brief Given two similary-typed variables and their type, swap them
 * @param x A variable to swap
 * @param y A variable to swap
 * @param T The type of x and y
 */
#define swap(x, y, T) do { T SWAP = x; x = y; y = SWAP; } while (0)

#if !(CL_COMPILER || TCC_COMPILER)

    /**
     * @brief Indicates to the compiler (if supported) that this branch is likely to occur and it should arrange code accordingly
     * @param x
     * @returns The truth value of x
     * @note
     * If the compiler does not support this feature, the macro does nothing and just returns x
     */
    #define likely(x)   __builtin_expect(!!(x), 1)

    /**
     * @brief Indicates to the compiler (if supported) that this branch is unlikely to occur and it should arrange code accordingly
     * @param x
     * @returns The truth value of x
     * @note
     * If the compiler does not support this feature, the macro does nothing and just returns x
     */
    #define unlikely(x) __builtin_expect(!!(x), 0)
#else
    #define likely(x) (x)
    #define unlikely(x) (x)
#endif

// constants section

/**
 * @brief Denotes that printf is referenced in this header, but does not require including stdio.h
 */
#define EXTERN_PRINTF extern int printf(const char *const _Format, ...)

/**
 * @brief When on PCC, sqrt() has to be re-implemented. This controls the number of loops that happens in that case.
 */
#define PCC_SQRT_ACCURACY 8

#define MAX_FACTORIAL_64 20
#define MAX_FACTORIAL_128 34
#define MAX_POW_10_16 10000U
#define POW_OF_MAX_POW_10_16 4
#define MAX_POW_10_32 1000000000UL
#define POW_OF_MAX_POW_10_32 9
#define MAX_POW_10_64 10000000000000000000ULL
#define POW_OF_MAX_POW_10_64 19
#define MAX_POW_10_128 ((uintmax_t) MAX_POW_10_64 * (uintmax_t) MAX_POW_10_64)
#define POW_OF_MAX_POW_10_128 38

#endif
