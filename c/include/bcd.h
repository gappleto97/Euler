/**
 * @file bcd.h
 * @author Gabe Appleton
 * @date 12 Oct 2019
 * @brief An implementation of arbitrary-precision binary-coded-decimal integers
 */
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "macros.h"

#if !PCC_COMPILER
    #include <stdlib.h>
    #include <math.h>
#else
    #include "./math.h"
#endif

#ifdef DOXYGEN
namespace c::include::bcd {
#endif

EXTERN_PRINTF;

/** @section types
 *  @{
 */
/** @defgroup bcd_types
 *  The types defined for BCD_ints
 *  @{
 */

/**
 * @brief An alias for uint8_t that shows it is intended to store packed BCD data
 */
typedef uint8_t packed_BCD_pair;

/**
 * @brief An enum that we return for BCD comparisons
 */
typedef enum    {
    EQUAL_TO,      /**< x == y */
    GREATER_THAN,  /**< x > y */
    LESS_THAN,     /**< x < y */
    NO_COMP        /**< x.nan || y.nan */
} comp_t;

/**
 * @brief An enum that we use to track errors in BCD integers
 */
typedef enum    {
    NON_ERR,    /**< there wasn't an error */
    ORIG_NAN,   /**< this is the original @ref BCD_nan "NaN" */
    IS_FREED,   /**< this BCD_int has been freed */
    ADD_NAN,    /**< you tried to add @ref BCD_nan "NaN" */
    SUB_NAN,    /**< you tried to subtract @ref BCD_nan "NaN" */
    MUL_NAN,    /**< you tried to multiply @ref BCD_nan "NaN" */
    DIV_NAN,    /**< you tried to divide @ref BCD_nan "NaN" */
    POW_NAN,    /**< you tried to use @ref BCD_nan "NaN" as an exponent or base */
    FACT_NAN,   /**< you tried to get the factorial of @ref BCD_nan "NaN" */
    SHIFT_NAN,  /**< you tried to shift @ref BCD_nan "NaN" */
    POW_NEG,    /**< you tried to use a negative exponent */
    FACT_NEG,   /**< you tried to get a negative factorial */
    DIV_ZERO,   /**< you tried to divide by zero */
    NO_MEM,     /**< you are out of memory to make new ints */
    NOT_SUPP,   /**< you tried to do a yet-unimplemented feature */
    RESERVED,   /**< reserved */
} BCD_error;

/**
 * @brief A little-endian, arbitrary-precision, binary-coded decimal number
 */
typedef struct BCD_int  {
    packed_BCD_pair *digits;   /**< the raw digits of the integer, DO NOT modify directly */
    size_t bcd_digits;         /**< the byte count of digits */
    size_t decimal_digits;     /**< the number of decimal digits in this integer */
    bool negative : 1;         /**< indicates the integer is negative */
    bool zero : 1;             /**< indicates the integer is 0 */
    bool even : 1;             /**< indicates the integer is even */
    bool constant : 1;         /**< indicates that the integer is a constant and will not be touched by free_bcd_int() */
    bool nan : 1;              /**< indicates that the integer is @ref BCD_nan "NaN" */
    // note: the following are 5 bits instead of 4 because some compilers assume they are signed
    BCD_error error : 5;       /**< this specifies why the BCD_int is @ref BCD_nan "NaN", if it is */
    BCD_error orig_error : 5;  /**< this specifies the oringal source of @ref BCD_nan "NaN", rather than the most recent */
} BCD_int;

/** @} */ //
/** @} */ // end of types

/** @section constants
 *  @{
 */
/** @defgroup bcd constants
 *  The frequently used constants in this file
 *  @{
 */

/** The BCD_one constant is used to represent the commonly used value one*/
const BCD_int BCD_one  = {(packed_BCD_pair *) "\x01", 1, 1, false, false, false, true, false, NON_ERR,  NON_ERR};
/** The BCD_zero constant is used to represent the commonly used value zero*/
const BCD_int BCD_zero = {(packed_BCD_pair *) NULL,   0, 0, false, true,  true,  true, false, NON_ERR,  NON_ERR};
/** The BCD_nan constant is used to represent the commonly used value NaN*/
const BCD_int BCD_nan  = {(packed_BCD_pair *) NULL,   0, 0, false, false, false, true, true,  ORIG_NAN, ORIG_NAN};

/** @} */ //
/** @} */ // end of constants

/** @section destructor
 *  @{
 */
/** @defgroup bcd_destructor
 *  The functions to safely dispose of a @ref BCD_int
 *  @{
 */

/**
 * if x is not constant, this sets all flags to 0, frees used memory, sets nan to true, and error to @ref IS_FREED
 * @param x the @ref BCD_int you would like to destruct
 */
void free_BCD_int(BCD_int *x);

/** @} */ //
/** @} */ // end of destructor

/** @section constructor
 *  @{
 */
/** @defgroup bcd_constructors
 *  The functions used to make new @ref BCD_int "BCD_ints"
 *  @{
 */

/**
 * @param a The C integer you would like to convert to a @ref BCD_int
 * @attention
 * All constructors may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @return The @ref BCD_int representation of a
 */
BCD_int new_BCD_int1  (const intmax_t a);

/**
 * @param a The C integer you would like to convert to a @ref BCD_int
 * @param negative The sign of the C integer
 * @attention
 * All constructors may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @return The @ref BCD_int representation of a
 */
BCD_int new_BCD_int2  (uintmax_t a,                    const bool negative);

/**
 * @param a The @ref BCD_int you would like to copy
 * @attention
 * All constructors may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @return A copy of a
 */
BCD_int copy_BCD_int  (BCD_int a);

/**
 * @param str The BCD bytes you would like to copy from
 * @param chars The number of bytes
 * @param negative The sign you would like to apply to the resulting @ref BCD_int
 * @param little_endian The byte order of the fed data (little endian is preferred)
 * @attention
 * All constructors may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @return A @ref BCD_int representation of the given data
 */
BCD_int BCD_from_bytes(const unsigned char *const str, const size_t chars,  const bool negative, const bool little_endian);

/**
 * @param str The ASCII string you would like to make an integer from, must be decimal numbers (0x30 - 0x39)
 * @param digits The number of digits
 * @param negative The sign you would like to apply to the resulting @ref BCD_int
 * @attention
 * All constructors may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @return A @ref BCD_int representation of the given data
 */
BCD_int BCD_from_ascii(const char *const str,          const size_t digits, const bool negative);

/**
 * @brief A shorthand way of making a @ref BCD_int error object
 * @param error The operation which generated the current error
 * @param orig_error The operation which initially started generating errors
 * @attention
 * All constructors may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @return A @ref BCD_nan "NaN" value which has the associated error codes
 */
BCD_int bcd_error     (const BCD_error error,          const BCD_error orig_error);

/** @} */ //
/** @} */ // end of constructor

/** @section operators
 *  @{
 */
/** @defgroup bcd_operators
 *  The functions that implement math operators using only @ref BCD_int objects
 *  @{
 */

/**
 * @brief Get the truth value of a @ref BCD_int without relying on internals
 * @param x The @ref BCD_int in question
 * @return A bool indicating if the @ref BCD_int is non-zero
 */
bool    bool_bcd     (const BCD_int x);

/**
 * @brief Get the logical not of a @ref BCD_int without relying on internals
 * @param x The @ref BCD_int in question
 * @return A bool indicating if the @ref BCD_int is zero
 */
bool    not_bcd      (const BCD_int x);

/**
 * @brief Compare two @ref BCD_int "BCD_ints"
 * @param x The left-hand @ref BCD_int
 * @param y The right-hand @ref BCD_int
 * @return An @ref comp_t reflecting the comparison
 * @note
 * This will return @ref NO_COMP if either value is @ref BCD_nan "NaN"
 */
comp_t  cmp_bcd      (const BCD_int x, const BCD_int y);

/**
 * @brief Change the sign of a @ref BCD_int without relying on internals
 * @param x The @ref BCD_int in question
 * @param no_copy If false, this function returns a modified copy of x instead of a modified version of x
 * @param negative The sign you would like to apply
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM (avoid with no_copy)
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @return Either a modified copy of x or a modified version of x
 */
BCD_int sign_bcd     (BCD_int x,       const bool no_copy, const bool negative);

/**
 * @brief Get the absolute value of a @ref BCD_int without relying on internals
 * @param x The @ref BCD_int in question
 * @param no_copy If false, this function returns a modified copy of x instead of a modified version of x
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM (avoid with no_copy)
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @return Either a positive copy of x or a positive version of x
 */
BCD_int abs_bcd      (BCD_int x,       const bool no_copy);

/**
 * @brief Get the negative absolute value of a @ref BCD_int without relying on internals
 * @param x The @ref BCD_int in question
 * @param no_copy If false, this function returns a modified copy of x instead of a modified version of x
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM (avoid with no_copy)
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @return Either a negative copy of x or a negative version of x
 */
BCD_int neg_bcd      (BCD_int x,       const bool no_copy);

/**
 * @brief Get the opposite value of a @ref BCD_int without relying on internals
 * @param x The @ref BCD_int in question
 * @param no_copy If false, this function returns a modified copy of x instead of a modified version of x
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM (avoid with no_copy)
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @return Either an opposite copy of x or an opposite version of x
 */
BCD_int opp_bcd      (BCD_int x,       const bool no_copy);

/**
 * @brief Add two @ref BCD_int "BCD_ints"
 * @param x The left-hand @ref BCD_int
 * @param y The right-hand @ref BCD_int
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @return A @ref BCD_int reflecting the sum of the arguments
 */
BCD_int add_bcd      (BCD_int x,       const BCD_int y);

/**
 * @brief Increment a @ref BCD_int
 * @param x The @ref BCD_int to increment
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @return A @ref BCD_int one greater than x
 */
BCD_int inc_bcd      (const BCD_int x);

/**
 * @brief Subtract two @ref BCD_int "BCD_ints"
 * @param x The left-hand @ref BCD_int
 * @param y The right-hand @ref BCD_int
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @return A @ref BCD_int reflecting the difference of the arguments
 */
BCD_int sub_bcd      (BCD_int x,       const BCD_int y);

/**
 * @brief Decrement a @ref BCD_int
 * @param x The @ref BCD_int to decrement
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @return A @ref BCD_int one less than x
 */
BCD_int dec_bcd      (const BCD_int x);

/**
 * @brief Multiply two @ref BCD_int "BCD_ints"
 * @param x The left-hand @ref BCD_int
 * @param y The right-hand @ref BCD_int
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @return A @ref BCD_int reflecting the product of the arguments
 */
BCD_int mul_bcd      (const BCD_int x, const BCD_int y);

/**
 * @brief Divide two @ref BCD_int "BCD_ints"
 * @param x The left-hand @ref BCD_int
 * @param y The right-hand @ref BCD_int
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @attention
 * Returns @ref BCD_nan "NaN" with error set to @ref DIV_ZERO if y is 0
 * @return A @ref BCD_int reflecting the quotient of the arguments
 */
BCD_int div_bcd      (const BCD_int x, const BCD_int y);

/**
 * @brief Modulo divide two @ref BCD_int "BCD_ints"
 * @param x The left-hand @ref BCD_int
 * @param y The right-hand @ref BCD_int
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @attention
 * Returns @ref BCD_nan "NaN" with error set to @ref DIV_ZERO if y is 0
 * @return A @ref BCD_int reflecting the remainder of the arguments
 */
BCD_int mod_bcd      (const BCD_int x, const BCD_int y);

/**
 * @brief Divide and modulo divide two @ref BCD_int "BCD_ints"
 * @param x The left-hand @ref BCD_int
 * @param y The right-hand @ref BCD_int
 * @param mod A pointer to the @ref BCD_int you would like to store the remainder
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @attention
 * Returns (and assigns to mod) @ref BCD_nan "NaN" with error set to @ref DIV_ZERO if y is 0
 * @return A @ref BCD_int reflecting the quotient of the arguments
 */
BCD_int divmod_bcd   (const BCD_int x, BCD_int y,          BCD_int *mod);

/**
 * @brief Raise x to the power of y
 * @param x The left-hand @ref BCD_int
 * @param y The right-hand @ref BCD_int
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @attention
 * Returns @ref BCD_nan "NaN" with error set to @ref POW_NEG if y < 0
 * @return A @ref BCD_int reflecting the yth power of x
 */
BCD_int pow_bcd      (const BCD_int x, const BCD_int y);

/**
 * @brief Get the factorial of x
 * @param x The @ref BCD_int in question
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @attention
 * Returns @ref BCD_nan "NaN" with error set to @ref FACT_NEG if x < 0
 * @return A @ref BCD_int reflecting the factorial of x
 */
BCD_int factorial_bcd(const BCD_int x);

/** @} */ //
/** @} */ // end of operators

/** @section in_place_operators
 *  @{
 */
/** @defgroup in_place_operators The functions that implement in-place math operators using only BCD_int objects
 *  @{
 */

/**
 * @brief Change the sign of a @ref BCD_int without relying on internals
 * @param x The @ref BCD_int in question
 * @param negative The sign you would like to apply
 * @attention
 * All in-place operators may "return" @ref BCD_nan "NaN" with error set to @ref NO_MEM (for sign modifiers this is only possible if x is a constant)
 * @attention
 * All in-place operators will "return" @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and on sign modifiers error is left untouched
 */
void isign_bcd     (BCD_int *x, const bool negative);

/**
 * @brief Get the absolute value of a @ref BCD_int without relying on internals
 * @param x The @ref BCD_int in question
 * @attention
 * All in-place operators may "return" @ref BCD_nan "NaN" with error set to @ref NO_MEM (for sign modifiers this is only possible if x is a constant)
 * @attention
 * All in-place operators will "return" @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and on sign modifiers error is left untouched
 */
void iabs_bcd      (BCD_int *x);

/**
 * @brief Get the negative absolute value of a @ref BCD_int without relying on internals
 * @param x The @ref BCD_int in question
 * @attention
 * All in-place operators may "return" @ref BCD_nan "NaN" with error set to @ref NO_MEM (for sign modifiers this is only possible if x is a constant)
 * @attention
 * All in-place operators will "return" @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and on sign modifiers error is left untouched
 */
void ineg_bcd      (BCD_int *x);

/**
 * @brief Get the opposite value of a @ref BCD_int without relying on internals
 * @param x The @ref BCD_int in question
 * @attention
 * All in-place operators may "return" @ref BCD_nan "NaN" with error set to @ref NO_MEM (for sign modifiers this is only possible if x is a constant)
 * @attention
 * All in-place operators will "return" @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and on sign modifiers error is left untouched
 */
void iopp_bcd      (BCD_int *x);

/**
 * @brief Add two @ref BCD_int "BCD_ints" and assign the result to x
 * @param x The left-hand @ref BCD_int
 * @param y The right-hand @ref BCD_int
 * @attention
 * All in-place operators may "return" @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All in-place operators will "return" @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 */
void iadd_bcd      (BCD_int *x, const BCD_int y);

/**
 * @brief Increment a @ref BCD_int and assign the result to x
 * @param x The left-hand @ref BCD_int
 * @attention
 * All in-place operators may "return" @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All in-place operators will "return" @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 */
void iinc_bcd      (BCD_int *x);

/**
 * @brief Subtract two @ref BCD_int "BCD_ints" and assign the result to x
 * @param x The left-hand @ref BCD_int
 * @param y The right-hand @ref BCD_int
 * @attention
 * All in-place operators may "return" @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All in-place operators will "return" @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 */
void isub_bcd      (BCD_int *x, const BCD_int y);

/**
 * @brief Decrement a @ref BCD_int and assign the result to x
 * @param x The left-hand @ref BCD_int
 * @attention
 * All in-place operators may "return" @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All in-place operators will "return" @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 */
void idec_bcd      (BCD_int *x);

/**
 * @brief Multiply two @ref BCD_int "BCD_ints" and assign the result to x
 * @param x The left-hand @ref BCD_int
 * @param y The right-hand @ref BCD_int
 * @attention
 * All in-place operators may "return" @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All in-place operators will "return" @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 */
void imul_bcd      (BCD_int *x, const BCD_int y);

/**
 * @brief Divide two @ref BCD_int "BCD_ints" and assign the result to x
 * @param x The left-hand @ref BCD_int
 * @param y The right-hand @ref BCD_int
 * @attention
 * All in-place operators may "return" @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All in-place operators will "return" @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @attention
 * "Returns" @ref BCD_nan "NaN" with error set to @ref DIV_ZERO if y is 0
 */
void idiv_bcd      (BCD_int *x, const BCD_int y);

/**
 * @brief Modulo divide two @ref BCD_int "BCD_ints" and assign the result to x
 * @param x The left-hand @ref BCD_int
 * @param y The right-hand @ref BCD_int
 * @attention
 * All in-place operators may "return" @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All in-place operators will "return" @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @attention
 * "Returns" @ref BCD_nan "NaN" with error set to @ref DIV_ZERO if y is 0
 */
void imod_bcd      (BCD_int *x, const BCD_int y);

/**
 * @brief Divide two @ref BCD_int "BCD_ints" then assign the quotient to x and remainder to y
 * @param x The left-hand @ref BCD_int
 * @param y The right-hand @ref BCD_int
 * @attention
 * All in-place operators may "return" @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All in-place operators will "return" @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @attention
 * "Returns" two @ref BCD_nan "BCD_nans" with error set to @ref DIV_ZERO if y is 0
 */
void idivmod_bcd   (BCD_int *x, BCD_int *y);

/**
 * @brief Raise x to the power of y then assign the result to x
 * @param x The left-hand @ref BCD_int
 * @param y The right-hand @ref BCD_int
 * @attention
 * All in-place operators may "return" @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All in-place operators will "return" @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @attention
 * "Returns" @ref BCD_nan "NaN" with error set to @ref POW_NEG if y < 0
 */
void ipow_bcd      (BCD_int *x, const BCD_int y);

/**
 * @brief Get the factorial of x then assign the result to x
 * @param x The @ref BCD_int in question
 * @attention
 * All in-place operators may "return" @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All in-place operators will "return" @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @attention
 * "Returns" @ref BCD_nan "NaN" with error set to @ref FACT_NEG if x < 0
 */
void ifactorial_bcd(BCD_int *x);

/** @} */ //
/** @} */ // end of in_place_operators

/** @section c_operators
 *  @{
 */
/** @defgroup bcd_c_operators
 *  The functions that implement math operators using at least some C-style integers
 *  @{
 */

/**
 * @brief Attempt to get the C-style absolute value of x
 * @param x The @ref BCD_int in question
 * @return Either the value of x OR -1 if x is @ref BCD_nan "NaN" or |x| would overflow
 */
uintmax_t abs_bcd_cuint  (const BCD_int x);

/**
 * @brief Attempt to get the C-style signed representation of x
 * @param x The @ref BCD_int in question
 * @return Either the value of x OR INTMAX_MIN if x is @ref BCD_nan "NaN" or |x| would overflow
 */
intmax_t  val_bcd_cint   (const BCD_int x);

/**
 * @brief compare a @ref BCD_int with a C-style signed integer
 * @param x The @ref BCD_int in question
 * @param y The signed integer you would like to compare with
 * @return A @ref comp_t indicating the difference between x and y
 * @note Returns @ref NO_COMP if x is @ref BCD_nan "NaN"
 * @see cmp_bcd
 */
comp_t    cmp_bcd_cint   (const BCD_int x,   const intmax_t y);

/**
 * @brief compare a @ref BCD_int with a C-style unsigned integer
 * @param x The @ref BCD_int in question
 * @param y The signed integer you would like to compare with
 * @return A @ref comp_t indicating the difference between x and y
 * @note Returns @ref NO_COMP if x is @ref BCD_nan "NaN"
 * @see cmp_bcd
 */
comp_t    cmp_bcd_cuint  (const BCD_int x,   const uintmax_t y);

/**
 * @brief Multiply a @ref BCD_int by a C-style signed integer
 * @param x The @ref BCD_int in question
 * @param y The C-style integer to multiply by
 * @return The product of x and y
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 */
BCD_int   mul_bcd_cint   (const BCD_int x,   const intmax_t y);

/**
 * @brief Multiply a @ref BCD_int by a C-style unsigned integer
 * @param x The @ref BCD_int in question
 * @param y The C-style integer to multiply by
 * @return The product of x and y
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 */
BCD_int   mul_bcd_cuint  (const BCD_int x,   uintmax_t y);

/**
 * @brief Multiply a @ref BCD_int by a power of ten
 * @param x The @ref BCD_int you'd like to multiply
 * @param tens The power of ten to multiply by (in the form of \f$(10^{\texttt{tens}})\f$)
 * @return The result of \f$(x \cdot 10^{\texttt{tens}})\f$
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 */
BCD_int   mul_bcd_pow_10 (const BCD_int x,   const uintmax_t tens);

/**
 * @brief Multiply a @ref BCD_int by a power of ten
 * @param a The @ref BCD_int you'd like to multiply
 * @param tens The power of ten to multiply by (in the form of \f$(10^{\texttt{tens}})\f$)
 * @return The result of \f$(x \cdot 10^{\texttt{tens}})\f$
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @note
 * This is just an alias for @ref mul_bcd_pow_10
 */
BCD_int   shift_bcd_left (const BCD_int a,   const uintmax_t tens);

/**
 * @brief Divide a @ref BCD_int by a power of ten
 * @param x The @ref BCD_int you'd like to divide
 * @param tens The power of ten to divide by (in the form of \f$(10^{\texttt{tens}})\f$)
 * @return The result of \f$(x \div 10^{\texttt{tens}})\f$
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @todo
 * This function is not yet supported if tens is odd
 */
BCD_int   div_bcd_pow_10 (const BCD_int x,   const uintmax_t tens);

/**
 * @brief Divide a @ref BCD_int by a power of ten
 * @param a The @ref BCD_int you'd like to divide
 * @param tens The power of ten to divide by (in the form of \f$(10^{\texttt{tens}})\f$)
 * @return The result of \f$(x \div 10^{\texttt{tens}})\f$
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @note
 * This is just an alias for @ref div_bcd_pow_10
 * @todo
 * This function is not yet supported if tens is odd
 */
BCD_int   shift_bcd_right(const BCD_int a,   const uintmax_t tens);

/**
 * @brief Raise a C-style integer to the power of another C-style signed integer
 * @param x The base
 * @param y The exponent
 * @return The yth power of x in the form of a @ref BCD_int
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 */
BCD_int   pow_cint_cuint (const intmax_t x,  uintmax_t y);

/**
 * @brief Raise a C-style integer to the power of another C-style unsigned integer
 * @param x The base
 * @param y The exponent
 * @return The yth power of x in the form of a @ref BCD_int
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 */
BCD_int   pow_cuint_cuint(const uintmax_t x, uintmax_t y);

/** @} */ //
/** @} */ // end of bcd_c_operators

/** @section in_place_c_operators
 *  @{
 */
/** @defgroup in_place_bcd_c_operators
 *  The functions that implement in-place math operators using at least some C-style integers
 *  @{
 */

/**
 * @brief Multiply in-place a @ref BCD_int by a C-style signed integer
 * @param x The @ref BCD_int in question
 * @param y The C-style integer to multiply by
 * @attention
 * All in-place operators may "return" @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All in-place operators will "return" @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 */
void imul_bcd_cint   (BCD_int *x, const intmax_t y);

/**
 * @brief Multiply in-place a @ref BCD_int by a C-style unsigned integer
 * @param x The @ref BCD_int in question
 * @param y The C-style integer to multiply by
 * @attention
 * All in-place operators may "return" @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All in-place operators will "return" @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 */
void imul_bcd_cuint  (BCD_int *x, const uintmax_t y);

/**
 * @brief Multiply in-place a @ref BCD_int by a power of ten
 * @param x The @ref BCD_int in question
 * @param tens The power of ten you'd like to multiply by (in the form of \f$(10^{\texttt{tens}})\f$)
 * @see mul_bcd_pow_10
 * @attention
 * All in-place operators may "return" @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All in-place operators will "return" @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 */
void imul_bcd_pow_10 (BCD_int *x, const uintmax_t tens);

/**
 * @brief Multiply in-place a @ref BCD_int by a power of ten
 * @param a The @ref BCD_int in question
 * @param tens The power of ten you'd like to multiply by (in the form of \f$(10^{\texttt{tens}})\f$)
 * @see mul_bcd_pow_10
 * @attention
 * All in-place operators may "return" @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All in-place operators will "return" @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 */
void ishift_bcd_left (BCD_int *a, const uintmax_t tens);

/**
 * @brief Divide in-place a @ref BCD_int by a power of ten
 * @param x The @ref BCD_int in question
 * @param tens The power of ten you'd like to divide by (in the form of \f$(10^{\texttt{tens}})\f$)
 * @see div_bcd_pow_10
 * @attention
 * All in-place operators may "return" @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All in-place operators will "return" @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @todo
 * This function is not yet supported for odd values of tens
 */
void idiv_bcd_pow_10 (BCD_int *x, const uintmax_t tens);

/**
 * @brief Divide in-place a @ref BCD_int by a power of ten
 * @param a The @ref BCD_int in question
 * @param tens The power of ten you'd like to divide by (in the form of \f$(10^{\texttt{tens}})\f$)
 * @see div_bcd_pow_10
 * @attention
 * All in-place operators may "return" @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All in-place operators will "return" @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @todo
 * This function is not yet supported for odd values of tens
 */
void ishift_bcd_right(BCD_int *a, const uintmax_t tens);

/** @} */ //
/** @} */ // end of in_place_bcd_c_operators

/** @section utility
 *  @{
 */
/** @defgroup bcd_utility
 *  A couple of utility functions for @ref BCD_int
 *  @{
 */

/**
 * @brief Multiply a pair of BCD bytes
 * @param ab The first byte, which gets split into nibbles a and b
 * @param cd The second byte, which gets split into nibbles c and d
 * @returns \f$100 \cdot a \cdot c + 10 \cdot (a \cdot d + b \cdot c) + b \cdot d\f$
 */
uint16_t mul_dig_pair(const packed_BCD_pair ab, const packed_BCD_pair cd);

/**
 * @brief Print a @ref BCD_int to the screen
 * @param x The @ref BCD_int you'd like to print
 */
void     print_bcd   (const BCD_int x);

/**
 * @brief Print a @ref BCD_int to the screen and append a newline
 * @param x The @ref BCD_int you'd like to print
 */
void     print_bcd_ln(const BCD_int x);

/** @} */ //
/** @} */ // end of bcd_utility

inline void free_BCD_int(BCD_int *x)    {
    if (x->error != IS_FREED && !x->constant)   {
        free(x->digits);
        x->digits = NULL;
        x->nan = true;
        x->orig_error = x->error = IS_FREED;
        x->zero = x->even = x->negative = false;
    }
}

BCD_int new_BCD_int1(const intmax_t a)  {
    if (a < 0)
        return new_BCD_int2((uintmax_t) (-a), true);
    return new_BCD_int2((uintmax_t) a, false);
}

BCD_int new_BCD_int2(uintmax_t a, const bool negative)  {
    BCD_int c;
    #if !PCC_COMPILER
        c.decimal_digits = ceil(log10(a + 1));
    #else
        c.decimal_digits = imprecise_log10(a + 1);
    #endif
    c.bcd_digits = (c.decimal_digits + 1) / 2;
    c.digits = (packed_BCD_pair *) malloc(sizeof(packed_BCD_pair) * c.bcd_digits);
    if (unlikely(c.digits == NULL))
        return bcd_error(NO_MEM, NO_MEM);
    c.negative = negative;
    c.even = !(a % 2);
    c.constant = c.nan = false;
    c.zero = !a;
    c.orig_error = c.error = NON_ERR;
    for (size_t i = 0; i < c.bcd_digits; i++)   {
        c.digits[i] = (((a % 100) / 10) << 4) | (a % 10);
        a /= 100;
    }
    return c;
}

inline BCD_int copy_BCD_int(BCD_int a)  {
    if (likely(a.digits != NULL))   {
        packed_BCD_pair *data = a.digits;
        a.digits = (packed_BCD_pair *) malloc(sizeof(packed_BCD_pair) * a.bcd_digits);
        if (unlikely(a.digits == NULL))
            return bcd_error(NO_MEM, NO_MEM);
        memcpy(a.digits, data, a.bcd_digits);
    }
    return a;
}

BCD_int BCD_from_bytes(const unsigned char *const str, const size_t chars, const bool negative, const bool little_endian)   {
    // converts a BCD-formatted bytestring to a little-endian BCD int
    if (!chars || str == NULL)
        return BCD_zero;
    BCD_int c = BCD_one;
    size_t i;
    c.constant = false;
    c.negative = negative;
    c.digits = (packed_BCD_pair *) malloc(sizeof(packed_BCD_pair) * chars);
    if (unlikely(c.digits == NULL))
        return bcd_error(NO_MEM, NO_MEM);
    if (little_endian)  {
        memcpy(c.digits, str, chars);
    }
    else    {
        size_t j;
        for (i = 0, j = chars - 1; i < chars; i++, j--) {
            c.digits[i] = str[j];
        }
    }
    for (i = chars - 1; i != -1; i--)   {
        if (c.digits[i])    {
            c.bcd_digits = i + 1;
            if (c.digits[i] & 0xF0)
                c.decimal_digits = i * 2 + 1;
            else
                c.decimal_digits = i * 2;
            break;
        }
    }
    if (unlikely(i == -1))  {
        free_BCD_int(&c);
        return BCD_zero;
    }
    c.even = !(c.digits[0] % 2);
    return c;
}

BCD_int BCD_from_ascii(const char *const str, const size_t digits, const bool negative) {
    // packs an ASCII digit string into big-endian bytes, then runs through BCD_from_bytes()
    const size_t length = (digits + 1) / 2;
    size_t i, j;
    unsigned char *bytes = (unsigned char *) malloc(sizeof(unsigned char) * length);
    if (unlikely(bytes == NULL))
        return bcd_error(NO_MEM, NO_MEM);
    if ((j = i = digits % 2))
        bytes[0] = str[0] - '0';
    for (; i < length; i++, j += 2) {
        bytes[i] = ((str[j] - '0') << 4) | ((str[j + 1] - '0') & 0xF);
    }
    BCD_int ret = BCD_from_bytes(bytes, length, negative, false);
    free(bytes);
    return ret;
}

inline BCD_int bcd_error(const BCD_error error, const BCD_error orig_error)  {
    BCD_int ret = BCD_nan;
    ret.error = error;
    ret.orig_error = orig_error;
    return ret;
}

comp_t cmp_bcd(const BCD_int x, const BCD_int y)    {
    // returns:
    // NO_COMP      if (x.nan || y.nan)
    // GREATER_THAN if x > y
    // LESS_THAN    if x < y
    // EQUAL_TO     else
    if (unlikely(x.nan || y.nan))
        return NO_COMP;
    if (x.negative != y.negative)
        return (x.negative) ? LESS_THAN : GREATER_THAN;
    if (x.decimal_digits != y.decimal_digits)  {
        if (x.decimal_digits > y.decimal_digits)
            return (x.negative) ? LESS_THAN : GREATER_THAN;
        return (x.negative) ? GREATER_THAN : LESS_THAN;
    }
    for (size_t i = x.bcd_digits - 1; i != -1; i--) {
        if (x.digits[i] != y.digits[i]) {
            if (x.negative)
                return (x.digits[i] > y.digits[i]) ? LESS_THAN : GREATER_THAN;
            return (x.digits[i] > y.digits[i]) ? GREATER_THAN : LESS_THAN;
        }
    }
    return EQUAL_TO;
}

inline bool bool_bcd(const BCD_int x)   {
    return x.zero;
}

inline bool not_bcd(const BCD_int x)    {
    return !x.zero;
}

inline BCD_int sign_bcd(BCD_int x, const bool no_copy, const bool negative) {
    if (!x.nan)
        x.negative = negative;
    return (no_copy) ? x : copy_BCD_int(x);
}

inline BCD_int abs_bcd(BCD_int x, const bool no_copy)   {
    return sign_bcd(x, no_copy, false);
}

inline BCD_int neg_bcd(BCD_int x, const bool no_copy)   {
    return sign_bcd(x, no_copy, true);
}

inline BCD_int opp_bcd(BCD_int x, const bool no_copy)   {
    return sign_bcd(x, no_copy, !x.negative);
}

BCD_int add_bcd(BCD_int x, const BCD_int y) {
    // performing this on two n-digit numbers will take O(n) time
    if (unlikely(x.nan || y.nan))
        return bcd_error(ADD_NAN, (x.nan) ? x.orig_error : y.orig_error);
    if (unlikely(x.zero && y.zero))
        return BCD_zero;
    if (unlikely(x.zero))
        return copy_BCD_int(y);
    if (unlikely(y.zero))
        return copy_BCD_int(x);
    if (x.negative != y.negative)
        return sub_bcd(x, opp_bcd(y, true));
        // if signs don't match, absolute value would go down.
        // that means we need to flip y's sign and move through sub_bcd()
    BCD_int z;
    size_t i, min_digits = min(x.bcd_digits, y.bcd_digits), max_digits = max(x.bcd_digits, y.bcd_digits);
    bool overflow = z.nan = z.constant = z.zero = false;  // result can't be zero because x and y are non-zero and share a sign
    z.orig_error = z.error = NON_ERR;
    z.negative = x.negative;  // we know this is also y.negative
    z.digits = (packed_BCD_pair *) malloc(sizeof(packed_BCD_pair) * (max_digits + 1));
    if (unlikely(z.digits == NULL))
        return bcd_error(NO_MEM, NO_MEM);
    packed_BCD_pair a, b, c;
    for (i = 0; i < min_digits; i++) {
        a = x.digits[i];
        b = y.digits[i];
        if (!(overflow || a))   {
            c = b;
            overflow = false;
        }
        else if (!(overflow || b))  {
            c = a;
            overflow = false;
        }
        else    {
            b += overflow;  // incorporate overflow from last pair
            #if (!defined(NO_ASSEMBLY) && X86_COMPILER)  // if on these architectures, there's assembly tricks to adjust BCD in-silicon
                #if CL_COMPILER
                    // CL compiler has a different syntax for inline assembly, does a lot less lifting
                    // note that the syntax here is: op [dest [, src]]
                    // brackets indicate a C symbol is referenced rather than a register
                    __asm   {
                        mov al, [a];         // move C symbol a to register al
                        add al, [b];         // add C symbol b to register al
                        daa;                 // have the CPU make sure register al contains valid, packed BCD digits
                        setc [overflow];     // set C symbol overflow to contain the carry bit, set by daa
                        mov [c], al;         // move register al to C symbol c
                    }
                #else
                    // this is the standard GCC/LLVM syntax for it
                    // note that the syntax here is: op [[src, ] dest]
                    // %\d indicates a C symbol is referenced, see the lookups at the end of code for which
                    __asm__(
                        "add %3, %%al;"    // add the register containing b to al
                        "daa;"             // have the CPU make sure register al contains valid, packed BCD digits
                        "setc %1;"         // set the register containing overflow to hold the carry bit, set by daa
                                           // this next section tells the compiler what to do after execution
                      : "=a" (c),          // store the contents of register al in symbol c
                        "=rgm" (overflow)  // and a general register or memory location gets assigned to symbol overflow (referenced as %1)
                                           // then below tells the compiler what our inputs are
                      : "a" (a),           // symbol a should get dumped to register al
                        "rgm" (b)          // and symbol b in a general register or memory location (referenced as %3)
                    );
                #endif
            #else  // otherwise fall back to doing it in C
                c = a + b;                        // set c to be the result of (a + b) % 0x100
                if ((overflow = (a > 0x99 - b)))  // if c would overflow the decimal range
                    c += 0x60;                    // and add 0x60 to make a decimal digit
                if (((a & 0xF) + (b & 0xF)) > 9)  // if the lower nibble be bigger than 9
                    c += 0x06;                    // add 6 to make a decimal digit
            #endif
            }
        z.digits[i] = c;
    }
    if (x.bcd_digits < y.bcd_digits)
        x = y;
    for (; overflow && i < max_digits; i++) {  // while there's overflow and digits, continue adding
        a = x.digits[i] + overflow;
        if ((a & 0x0F) == 0x0A)  // since all that's left is overflow, we don't need to check ranges
            a += 0x06;
        if ((overflow = ((a & 0xF0) == 0xA0)))
            a += 0x60;
        z.digits[i] = a;
    }
    if (i < max_digits)  // if there's no more overflow, but still digits left, copy directly
        memcpy(z.digits + i, x.digits + i, max_digits - i);
    z.bcd_digits = max_digits + overflow;
    if ((z.digits[max_digits] = overflow))
        z.decimal_digits = max_digits * 2 + 1;
    else if (z.digits[max_digits - 1] & 0xF0)
        z.decimal_digits = max_digits * 2;
    else
        z.decimal_digits = max_digits * 2 - 1;
    z.even = !(z.digits[0] % 2);
    return z;
}

inline BCD_int inc_bcd(const BCD_int x) {
    return add_bcd(x, BCD_one);
}

BCD_int sub_bcd(BCD_int x, const BCD_int y) {
    if (unlikely(x.nan || y.nan))
        return bcd_error(SUB_NAN, (x.nan) ? x.orig_error : y.orig_error);
    if (unlikely(x.zero && y.zero))
        return BCD_zero;
    if (unlikely(x.zero))
        return opp_bcd(y, false);
    if (unlikely(y.zero))
        return copy_BCD_int(x);
    if (x.negative != y.negative)
        return add_bcd(x, opp_bcd(y, true));
        // if signs don't match, absolute value would go up.
        // that means we need to flip y's sign and move through add_bcd()
    signed char cmp = cmp_bcd(x, y);
    BCD_int z;
    if (unlikely((z.zero = !cmp)))  // x and y can't be NaN, so this is safe
        return BCD_zero;
    z.negative = (cmp == -1);
    bool carry = z.nan = z.constant = false;
    z.orig_error = z.error = NON_ERR;
    size_t i;
    const size_t min_digits = min(x.bcd_digits, y.bcd_digits), max_digits = max(x.bcd_digits, y.bcd_digits);
    z.digits = (packed_BCD_pair *) malloc(sizeof(packed_BCD_pair) * max_digits);
    if (unlikely(z.digits == NULL))
        return bcd_error(NO_MEM, NO_MEM);
    packed_BCD_pair a, b, c;
    for (i = 0; i < min_digits; i++) {
        a = x.digits[i];
        b = y.digits[i];
        b += carry;  // incorporate carry from last pair
        #if (!defined(NO_ASSEMBLY) && X86_COMPILER)  // if on these architectures, there's assembly tricks to adjust BCD in-silicon
            #if CL_COMPILER
                // CL compiler has a different syntax for inline assembly, does a lot less lifting
                // note that the syntax here is: op [dest [, src]]
                // brackets indicate a C symbol is referenced rather than a register
                __asm   {
                    mov al, [a];         // move C symbol a to register al
                    sub al, [b];         // add C symbol b to register al
                    das;                 // have the CPU make sure register al contains valid, packed BCD digits
                    setc [carry];        // set C symbol carry to contain the carry bit, set by daa
                    mov [c], al;         // move register al to C symbol c
                }
            #else
                // this is the standard GCC/LLVM syntax for it
                // note that the syntax here is: op [[src, ] dest]
                // %\d indicates a C symbol is referenced, see the lookups at the end of code for which
                __asm__(
                    "sub %3, %%al;"    // add the register containing b to al
                    "das;"             // have the CPU make sure register al contains valid, packed BCD digits
                    "setc %1;"         // set the register containing carry to hold the carry bit, set by daa
                                        // this next section tells the compiler what to do after execution
                    : "=a" (c),          // store the contents of register al in symbol c
                    "=rgm" (carry)     // and a general register or memory location gets assigned to symbol carry (referenced as %1)
                                        // then below tells the compiler what our inputs are
                    : "a" (a),           // symbol a should get dumped to register al
                    "rgm" (b)          // and symbol b in a general register or memory location (referenced as %3)
                );
            #endif
        #else  // otherwise fall back to doing it in C
            c = a - b;                        // set c to be the result of (a - b) % 0x100
            if ((carry = (c & 0xF0) > 0x99))  // if c would overflow the decimal range
                c -= 0x60;                    // and subtract 0x60 to make a decimal digit
            if ((c & 0x0F) > 9)               // if the lower nibble be bigger than 9
                c -= 0x06;                    // subtract 6 to make a decimal digit
        #endif
        z.digits[i] = c;
    }
    if (x.bcd_digits < y.bcd_digits)
        x = y;
    for (; carry && i < max_digits; i++) {  // while there's carry and digits, continue adding
        a = x.digits[i] - carry;
        if ((a & 0x0F) == 0x0F)  // since all that's left is carry, we don't need to check ranges
            a -= 0x06;
        if ((carry = ((a & 0xF0) == 0xF0)))
            a -= 0x60;
        z.digits[i] = a;
    }
    if (i < max_digits)  // if there's no more carry, but still digits left, copy directly
        memcpy(z.digits + i, x.digits + i, max_digits - i);
    while (--i != -1)   {
        if (z.digits[i])    {
            z.bcd_digits = i + 1;
            if (z.digits[i] & 0xF0)
                z.decimal_digits = 2 * z.bcd_digits;
            else
                z.decimal_digits = 2 * z.bcd_digits - 1;
            z.even = !(z.digits[0] % 2);
            return z;
        }
    }
    free_BCD_int(&z);  // should not get here, but just in case
    return BCD_zero;
}


inline BCD_int dec_bcd(const BCD_int x) {
    return sub_bcd(x, BCD_one);
}

BCD_int mul_bcd(const BCD_int x, const BCD_int y)   {
    // multiplies two BCD ints by breaking them down into their component bytes and adding the results
    // this takes O(log_100(x) * log_100(y) * log_100(xy)) time
    BCD_int answer = BCD_zero, addend;
    if (unlikely(x.nan || y.nan))
        return bcd_error(MUL_NAN, (x.nan) ? x.orig_error : y.orig_error);
    if (unlikely(x.zero || y.zero))
        return answer;
    size_t i, j;
    uint16_t staging;
    uintmax_t ipow_10 = 0, pow_10;
    for (i = 0; i < x.bcd_digits; i++, ipow_10 += 2)  {
        for (j = 0, pow_10 = ipow_10; j < y.bcd_digits; j++, pow_10 += 2) {
            if ((staging = mul_dig_pair(x.digits[i], y.digits[j])) == 0)
                continue;
            addend = new_BCD_int2(staging, false);
            if (likely(pow_10))
                imul_bcd_pow_10(&addend, pow_10);  // this was not added to performance analysis
            iadd_bcd(&answer, addend);
            free_BCD_int(&addend);
        }
    }
    answer.negative = !(x.negative == y.negative);
    return answer;
}

inline BCD_int div_bcd(const BCD_int x, const BCD_int y)    {
    BCD_int mod, div = divmod_bcd(x, y, &mod);
    free_BCD_int(&mod);
    return div;
}

inline BCD_int mod_bcd(const BCD_int x, const BCD_int y)    {
    BCD_int mod, div = divmod_bcd(x, y, &mod);
    free_BCD_int(&div);
    return mod;
}

BCD_int divmod_bcd(const BCD_int x, BCD_int y, BCD_int *mod)    {
    // TODO: optimize this similar to mul_bcd_cuint() by stepping by 10s when possible
    BCD_int div = BCD_zero;
    if (unlikely(x.nan || y.nan || y.zero)) {
        BCD_error error, orig_error;
        orig_error = error = (y.zero) ? DIV_ZERO : DIV_NAN;
        if (x.nan)
            orig_error = x.orig_error;
        else if (y.nan)
            orig_error = y.orig_error;
        return (*mod = bcd_error(error, orig_error));
    }
    if (unlikely(x.zero))   {
        *mod = BCD_zero;
        return div;
    }
    bool x_negative = x.negative, y_negative = y.negative;
    BCD_int tmp = copy_BCD_int(x);
    tmp.negative = y.negative = false;
    while (!x.zero && cmp_bcd(tmp, y) != LESS_THAN) {  // x and y can't be NaN, so this is safe
        isub_bcd(&tmp, y);
        iinc_bcd(&div);
    }
    if ((div.negative = (x_negative != y_negative)))    {
        if (!tmp.zero)
            idec_bcd(&div);
        *mod = sub_bcd(y, tmp);
        free_BCD_int(&tmp);
    }
    else    {
        *mod = tmp;
    }
    mod->negative = y_negative;
    return div;
}

BCD_int pow_bcd(const BCD_int x, const BCD_int y)   {
    // this takes O(y * 2log_100(x)^3) time
    if (unlikely(x.nan || y.nan || y.negative)) {
        BCD_error error, orig_error;
        orig_error = error = (y.negative) ? POW_NEG : POW_NAN;
        if (x.nan)
            orig_error = x.orig_error;
        else if (y.nan)
            orig_error = y.orig_error;
        return bcd_error(error, orig_error);
    }
    BCD_int answer = BCD_one, tmp = copy_BCD_int(y);
    while (!(tmp.zero || tmp.nan))  {
        imul_bcd(&answer, x);
        idec_bcd(&tmp);
    }
    free_BCD_int(&tmp);
    return answer;
}

BCD_int factorial_bcd(const BCD_int x)  {
    if (unlikely(x.nan))
        return bcd_error(FACT_NAN, x.orig_error);
    if (unlikely(x.negative))
        return bcd_error(FACT_NEG, FACT_NEG);
    if (unlikely(x.zero || (x.decimal_digits == 1 && x.digits[0] == 1)))
        return BCD_one;
    BCD_int i = dec_bcd(x), ret = copy_BCD_int(x);
    while (!i.zero) {
        imul_bcd(&ret, i);
        idec_bcd(&i);
    }
    free_BCD_int(&i);
    return ret;
}

inline void isign_bcd(BCD_int *x, const bool negative)  {
    if (!x->nan && x->constant)
        *x = copy_BCD_int(*x);
    if (!x->nan)
        x->negative = negative;
}

inline void iabs_bcd(BCD_int *x)    {
    isign_bcd(x, false);
}

inline void ineg_bcd(BCD_int *x)    {
    isign_bcd(x, true);
}

inline void iopp_bcd(BCD_int *x)    {
    isign_bcd(x, !x->negative);
}

inline void iadd_bcd(BCD_int *x, const BCD_int y)   {
    BCD_int ret = add_bcd(*x, y);
    free_BCD_int(x);
    *x = ret;
}

inline void iinc_bcd(BCD_int *x)    {
    BCD_int ret = inc_bcd(*x);
    free_BCD_int(x);
    *x = ret;
}

inline void isub_bcd(BCD_int *x, const BCD_int y)   {
    BCD_int ret = sub_bcd(*x, y);
    free_BCD_int(x);
    *x = ret;
}

inline void idec_bcd(BCD_int *x)    {
    BCD_int ret = dec_bcd(*x);
    free_BCD_int(x);
    *x = ret;
}

inline void imul_bcd(BCD_int *x, const BCD_int y)   {
    BCD_int ret = mul_bcd(*x, y);
    free_BCD_int(x);
    *x = ret;
}

inline void idiv_bcd(BCD_int *x, const BCD_int y)   {
    BCD_int ret = div_bcd(*x, y);
    free_BCD_int(x);
    *x = ret;
}

inline void imod_bcd(BCD_int *x, const BCD_int y)   {
    BCD_int ret = mod_bcd(*x, y);
    free_BCD_int(x);
    *x = ret;
}

inline void idivmod_bcd(BCD_int *x, BCD_int *y)    {
    BCD_int mod, div = divmod_bcd(*x, *y, &mod);
    free_BCD_int(x);
    free_BCD_int(y);
    *x = div;
    *y = mod;
}

inline void ipow_bcd(BCD_int *x, const BCD_int y)   {
    BCD_int ret = pow_bcd(*x, y);
    free_BCD_int(x);
    *x = ret;
}

inline void ifactorial_bcd(BCD_int *x)   {
    BCD_int ret = factorial_bcd(*x);
    free_BCD_int(x);
    *x = ret;
}

uintmax_t abs_bcd_cuint(const BCD_int x)    {
    if (unlikely(x.nan || (x.decimal_digits > POW_OF_MAX_POW_10_64 + 2)))
        return -1;  // overflow likely, or invalid number
    uintmax_t answer = 0, pow_10 = 1;
    for (size_t i = 0; i < x.bcd_digits; i++, pow_10 *= 100) {
        answer += pow_10 * ((x.digits[i] & 0x0F) + 10 * (x.digits[i] >> 4));
    }
    return answer;
}

intmax_t val_bcd_cint(const BCD_int x)  {
    if (unlikely(x.nan || (x.decimal_digits > POW_OF_MAX_POW_10_64 + 1)))
        return INTMAX_MIN;
    uintmax_t answer = abs_bcd_cuint(x);
    if (answer > (uintmax_t) INTMAX_MAX)
        return INTMAX_MIN;
    return (intmax_t) ((x.negative) ? -answer : answer);
}

inline comp_t cmp_bcd_cint(const BCD_int x, const intmax_t y)   {
    if (y >= 0) {
        comp_t inverse_answer = cmp_bcd_cuint(opp_bcd(x, true), (uintmax_t) (-y));
        if (inverse_answer == GREATER_THAN)
            return LESS_THAN;
        if (inverse_answer == LESS_THAN)
            return GREATER_THAN;
        return inverse_answer;
    }
    return cmp_bcd_cuint(x, (uintmax_t) y);
}

comp_t cmp_bcd_cuint(const BCD_int x, const uintmax_t y)    {
    if (unlikely(x.nan))
        return NO_COMP;
    if (unlikely(x.zero))
        return (y) ? GREATER_THAN : EQUAL_TO;
    if (x.negative)
        return LESS_THAN;
    if (x.decimal_digits > POW_OF_MAX_POW_10_64 + 2)
        return GREATER_THAN;
    uintmax_t x_prime = abs_bcd_cuint(x);
    if (unlikely(x_prime == -1))
        return NO_COMP;
    if (x_prime > y)
        return GREATER_THAN;
    if (x_prime < y)
        return LESS_THAN;
    return EQUAL_TO;
}

inline BCD_int mul_bcd_cint(const BCD_int x, const intmax_t y)  {
    if (y < 0)
        return opp_bcd(mul_bcd_cuint(x, (uintmax_t) (-y)), true);
    return mul_bcd_cuint(x, (uintmax_t) y);
}

BCD_int mul_bcd_cuint(const BCD_int x, uintmax_t y) {
    // this takes roughly O(log(y) * log(x)) time, and is nearly linear if y is a multiple of 10
    // it works by breaking the multiplication down into groups of addition
    // full formula for performance is something like:
    // y = 10^a * b
    // f(i) = sum(n = 1 thru 19, (i % 10^(n + 1)) / 10^n) + i / 10^20
    // bool(i) = 1 if i else 0
    // time = O(bool(a) * log_100(x) + f(b) * log_100(xy))
    if (unlikely(x.nan))
        return bcd_error(MUL_NAN, x.orig_error);
    if (unlikely(!y || x.zero))
        return BCD_zero;
    unsigned char tens = 0;  // will up size when there's an 848-bit system
    BCD_int ret = BCD_zero, mul_by_power_10;
    while (y % 10 == 0) {
        y /= 10;  // first remove factors of ten
        ++tens;
    }
    if (tens)
        ret = mul_bcd_pow_10(x, tens);
    // then for decreasing powers of ten, batch additions
    for (uintmax_t p = MAX_POW_10_64, tens = POW_OF_MAX_POW_10_64; p > 1; p /= 10, --tens)  {
        while (y >= p)  {
            mul_by_power_10 = mul_bcd_pow_10(x, tens);
            iadd_bcd(&ret, mul_by_power_10);
            free_BCD_int(&mul_by_power_10);
            y -= p;
        }
    }
    while (y--) {
        iadd_bcd(&ret, x);  // then do simple addition
    }
    return ret;
}

BCD_int mul_bcd_pow_10(const BCD_int x, const uintmax_t tens)   {
    // this takes O(log_100(x)) time. Note that it's significantly faster if tens is even
    // returns x * 10^tens
    BCD_int ret;
    if (unlikely(x.nan))
        return bcd_error(SHIFT_NAN, x.orig_error);
    if (unlikely(x.zero))
        return BCD_zero;
    if (unlikely(!tens))
        return copy_BCD_int(x);
    ret.zero = ret.nan = ret.constant = false;
    ret.orig_error = ret.error = NON_ERR;
    ret.even = x.even || !!tens;
    ret.negative = x.negative;
    ret.decimal_digits = x.decimal_digits + tens;
    ret.bcd_digits = (ret.decimal_digits + 1) / 2;
    ret.digits = (packed_BCD_pair *) calloc(ret.bcd_digits, sizeof(packed_BCD_pair));
    if (unlikely(ret.digits == NULL))
        return bcd_error(NO_MEM, NO_MEM);
    if (tens % 2 == 0)  {
        // +--+--+    +--+--+--+
        // |23|01| -> ...|23|01|
        // +--+--+    +--+--+--+
        const size_t digit_diff = ret.bcd_digits - x.bcd_digits;
        memcpy(ret.digits + digit_diff, x.digits, x.bcd_digits);
    }
    else    {
        // +--+--+    +--+--+--+
        // |23|01| -> ...|30|12|
        // +--+--+    +--+--+--+
        // +--+--+    +--+--+--+--+
        // |34|12| -> ...|40|23|01|
        // +--+--+    +--+--+--+--+
        const size_t digit_diff = ret.bcd_digits - x.bcd_digits - ret.decimal_digits % 2;
        // note that digit_diff needs to be adjusted on this branch, so it can't be common
        ret.digits[digit_diff] = x.digits[0] << 4;
        for (size_t i = 1; i < x.bcd_digits; i++)   {
            ret.digits[i + digit_diff] = x.digits[i] << 4;
            ret.digits[i + digit_diff] |= x.digits[i - 1] >> 4;
        }
        ret.digits[x.bcd_digits + digit_diff] |= x.digits[x.bcd_digits - 1] >> 4;
    }
    return ret;
}

inline BCD_int shift_bcd_left(const BCD_int x, const uintmax_t tens)    {
    return mul_bcd_pow_10(x, tens);
}

BCD_int div_bcd_pow_10(const BCD_int a, const uintmax_t tens)   {
    if (unlikely(a.nan))
        return bcd_error(SHIFT_NAN, a.orig_error);
    if (unlikely(!tens))
        return copy_BCD_int(a);
    if (tens >= a.decimal_digits)
        return BCD_zero;
    BCD_int ret = a;
    ret.decimal_digits = a.decimal_digits - tens;
    if (tens % 2 == 0)  {
        ret.bcd_digits = a.bcd_digits - tens / 2;
        ret.digits = (packed_BCD_pair *) malloc(sizeof(packed_BCD_pair) * ret.bcd_digits);
        if (unlikely(ret.digits == NULL))
            return bcd_error(NO_MEM, NO_MEM);
        memcpy(ret.digits, a.digits + tens / 2, ret.bcd_digits);
    }
    else    {
        // TODO
        return bcd_error(NOT_SUPP, NOT_SUPP);
    }
    ret.even = !(ret.digits[0] % 2);
    return ret;
}

inline BCD_int shift_bcd_right(const BCD_int a, const uintmax_t tens)   {
    return div_bcd_pow_10(a, tens);
}

inline BCD_int pow_cuint_cuint(const uintmax_t x, uintmax_t y)  {
    // this takes roughly O(x * y * log_100(xy)) time
    BCD_int answer = BCD_one;
    while (y--) {
        imul_bcd_cuint(&answer, x);
    }
    return answer;
}

inline BCD_int pow_cint_cuint(const intmax_t x, uintmax_t y)    {
    // this takes roughly O(x * y * log_100(xy)) time
    BCD_int answer = BCD_one;
    while (y--) {
        imul_bcd_cint(&answer, x);
    }
    return answer;
}

inline void imul_bcd_cuint(BCD_int *x, const uintmax_t y)   {
    BCD_int ret = mul_bcd_cuint(*x, y);
    free_BCD_int(x);
    *x = ret;
}

inline void imul_bcd_cint(BCD_int *x, const intmax_t y) {
    BCD_int ret = mul_bcd_cint(*x, y);
    free_BCD_int(x);
    *x = ret;
}

inline void imul_bcd_pow_10(BCD_int *x, const uintmax_t tens)   {
    BCD_int ret = mul_bcd_pow_10(*x, tens);
    free_BCD_int(x);
    *x = ret;
}

inline void ishift_bcd_left(BCD_int *x, const uintmax_t tens)   {
    imul_bcd_pow_10(x, tens);
}

inline void idiv_bcd_pow_10(BCD_int *a, const uintmax_t tens)   {
    BCD_int ret = div_bcd_pow_10(*a, tens);
    free_BCD_int(a);
    *a = ret;
}

inline void ishift_bcd_right(BCD_int *a, const uintmax_t tens)  {
    idiv_bcd_pow_10(a, tens);
}

inline uint16_t mul_dig_pair(const packed_BCD_pair ab, const packed_BCD_pair cd)    {
    // multiplies two digits pairs and returns an unsigned C short. valid range is 0 thru 9801
    // first unpack the digits
    const uint8_t a = ab >> 4,
                  b = ab & 0xF,
                  c = cd >> 4,
                  d = cd & 0xF;
    // then solve by FOIL
    return 100 * a * c + 10 * (a * d + b * c) + b * d;
}

void print_bcd(const BCD_int x) {
    if (unlikely(x.nan))    {
        printf("NaN");
        return;
    }
    if (unlikely(x.zero))   {
        printf("0");
        return;
    }
    if (x.negative)
        printf("-");
    size_t i = x.bcd_digits - 1;
    printf("%x", x.digits[i--]);
    for (; i != -1; i--)    {
        printf("%02x", x.digits[i]);
    }
}

inline void print_bcd_ln(const BCD_int x)   {
    print_bcd(x);
    printf("\n");
}

#ifdef DOXYGEN
};
#endif