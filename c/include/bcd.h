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

#include <stdlib.h>
#include <math.h>

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
 * @brief A little-endian, arbitrary-precision, binary-coded-decimal number
 */
typedef struct BCD_int  {
    packed_BCD_pair *data;  /**< the raw data of the integer, DO NOT modify directly */
    size_t bcd_digits;      /**< the byte count of data */
    size_t decimal_digits;  /**< the number of decimal digits in this integer */
    bool negative : 1;      /**< indicates the integer is negative */
    bool zero : 1;          /**< indicates the integer is 0 */
    bool even : 1;          /**< indicates the integer is even */
    bool constant : 1;      /**< indicates that the integer is a constant and will not be touched by free_bcd_int() */
    bool nan : 1;           /**< indicates that the integer is @ref BCD_nan "NaN" */
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
const BCD_int BCD_one = {
    .data = (packed_BCD_pair *) "\x01",
    .bcd_digits = 1,
    .decimal_digits = 1,
    .constant = true
};  // note that non-specified values are initialized to NULL or 0

/** The BCD_zero constant is used to represent the commonly used value zero*/
const BCD_int BCD_zero = {
    .zero = true,
    .even = true,
    .constant = true
};  // note that non-specified values are initialized to NULL or 0

/** The BCD_nan constant is used to represent the commonly used value NaN*/
const BCD_int BCD_nan = {
    .constant = true,
    .nan = true,
    .error = ORIG_NAN,
    .orig_error = ORIG_NAN
};  // note that non-specified values are initialized to NULL or 0

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
 * @memberof BCD_int
 * @remark
 * This will take \f$Θ(1)\f$ time if x is constant, and will match the time of free() if not. It is assumed in other
 * time analyses that this function takes constant time, but note that that may not be true in every C implementation.
 * For instance, if your implementation were to clear memory when freed this method would be \f$Θ(\log_{100}(x))\f$.
 * @relates BCD_int
 */
void free_BCD_int(BCD_int *const x);

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
 * @param[in] a The C integer you would like to convert to a @ref BCD_int
 * @return The @ref BCD_int representation of a
 * @memberof BCD_int
 * @attention
 * All constructors may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @remark
 * This will take \f$Θ(\log_{100}(a))\f$ time, but for most purposes you can treat it as \f$O(1)\f$
 */
BCD_int new_BCD_int1(const intmax_t a);

/**
 * @param[in] a The C integer you would like to convert to a @ref BCD_int
 * @param[in] negative The sign of the C integer
 * @return The @ref BCD_int representation of a
 * @memberof BCD_int
 * @attention
 * All constructors may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @remark
 * This will take \f$Θ(\log_{100}(a))\f$ time, but for most purposes you can treat it as \f$O(1)\f$
 */
BCD_int new_BCD_int2(uintmax_t a, const bool negative);

/**
 * @param[in] a The @ref BCD_int you would like to copy
 * @return A copy of a
 * @memberof BCD_int
 * @attention
 * All constructors may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @remark
 * This will take \f$Θ(\log_{100}(a))\f$ time
 */
BCD_int copy_BCD_int(BCD_int a);

/**
 * @param[in] str The BCD bytes you would like to copy from
 * @param[in] chars The number of bytes
 * @param[in] negative The sign you would like to apply to the resulting @ref BCD_int
 * @param[in] little_endian The byte order of the fed data (little endian is preferred)
 * @return A @ref BCD_int representation of the given data
 * @memberof BCD_int
 * @remark
 * This will take \f$Θ(\texttt{chars})\f$ time, though it should be noted that little endian data will run significantly faster
 * @attention
 * All constructors may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 */
BCD_int BCD_from_bytes(const unsigned char *const str, const size_t chars, const bool negative, const bool little_endian);

/**
 * @param[in] str The ASCII string you would like to make an integer from, must be decimal numbers (0x30 - 0x39)
 * @param[in] digits The number of digits
 * @param[in] negative The sign you would like to apply to the resulting @ref BCD_int
 * @return A @ref BCD_int representation of the given data
 * @memberof BCD_int
 * @remark
 * This will take \f$Θ(\texttt{digits})\f$ time
 * @attention
 * All constructors may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 */
BCD_int BCD_from_ascii(const char *const str, const size_t digits, const bool negative);

/**
 * @brief A shorthand way of making a @ref BCD_int error object
 * @param[in] error The operation which generated the current error
 * @param[in] orig_error The operation which initially started generating errors
 * @return A @ref BCD_nan "NaN" value which has the associated error codes
 * @memberof BCD_int
 * @remark
 * This will take \f$Θ(1)\f$ time
 * @attention
 * All constructors may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 */
BCD_int bcd_error(const BCD_error error, const BCD_error orig_error);

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
 * @brief Compare two @ref BCD_int "BCD_ints"
 * @param[in] x The left-hand @ref BCD_int
 * @param[in] y The right-hand @ref BCD_int
 * @return An @ref comp_t reflecting the comparison
 * @memberof BCD_int
 * @retval @ref EQUAL_TO
 * @retval @ref GREATER_THAN
 * @retval @ref LESS_THAN
 * @retval @ref NO_COMP
 * This value is when compared to @ref BCD_nan "NaN"
 * @remark
 * This will take \f$Ω(1), O(\log_{100}(x))\f$ time. More specifically, it has a series of short-circuits that it will
 * take if possible, several of which are constant-time, but if those fail it will compare byte-by-byte.
 */
comp_t cmp_bcd(const BCD_int x, const BCD_int y);

/**
 * @brief Change the sign of a @ref BCD_int without relying on internals
 * @param[in] x The @ref BCD_int in question
 * @param[in] no_copy If false, this function returns a modified copy of x instead of a modified version of x
 * @param[in] negative The sign you would like to apply
 * @return Either a modified copy of x or a modified version of x
 * @memberof BCD_int
 * @remark
 * This will take \f$Θ(1)\f$ time if @c no_copy is true, and \f$Θ(\log_{100}(x))\f$ if it is false.
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM (avoid with no_copy)
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 */
BCD_int sign_bcd(BCD_int x, const bool no_copy, const bool negative);

/**
 * @brief Get the absolute value of a @ref BCD_int without relying on internals
 * @param[in] x The @ref BCD_int in question
 * @param[in] no_copy If false, this function returns a modified copy of x instead of a modified version of x
 * @return Either a positive copy of x or a positive version of x
 * @memberof BCD_int
 * @remark
 * This will take \f$Θ(1)\f$ time if @c no_copy is true, and \f$Θ(\log_{100}(x))\f$ if it is false.
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM (avoid with no_copy)
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @see sign_bcd
 */
BCD_int abs_bcd(const BCD_int x, const bool no_copy);

/**
 * @brief Get the negative absolute value of a @ref BCD_int without relying on internals
 * @param[in] x The @ref BCD_int in question
 * @param[in] no_copy If false, this function returns a modified copy of x instead of a modified version of x
 * @return Either a negative copy of x or a negative version of x
 * @memberof BCD_int
 * @remark
 * This will take \f$Θ(1)\f$ time if @c no_copy is true, and \f$Θ(\log_{100}(x))\f$ if it is false.
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM (avoid with no_copy)
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @see sign_bcd
 */
BCD_int neg_bcd(const BCD_int x, const bool no_copy);

/**
 * @brief Get the opposite value of a @ref BCD_int without relying on internals
 * @param[in] x The @ref BCD_int in question
 * @param[in] no_copy If false, this function returns a modified copy of x instead of a modified version of x
 * @return Either an opposite copy of x or an opposite version of x
 * @memberof BCD_int
 * @remark
 * This will take \f$Θ(1)\f$ time if @c no_copy is true, and \f$Θ(\log_{100}(x))\f$ if it is false.
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM (avoid with no_copy)
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @see sign_bcd
 */
BCD_int opp_bcd(const BCD_int x, const bool no_copy);

/**
 * @brief Add two @ref BCD_int "BCD_ints"
 * @param[in] x The left-hand @ref BCD_int
 * @param[in] y The right-hand @ref BCD_int
 * @return A @ref BCD_int reflecting the sum of the arguments
 * @memberof BCD_int
 * @remark
 * This will take \f$O(\log_{100}(max(x, y)))\f$ time
 * @note
 * Implementation details:
 * @note
 * This function works closely with @ref sub_bcd. If absolute value would go down from this operation, it is handed off to that method.
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 */
BCD_int add_bcd(const BCD_int x, const BCD_int y);

/**
 * @brief Increment a @ref BCD_int
 * @param[in] x The @ref BCD_int to increment
 * @return A @ref BCD_int one greater than x
 * @memberof BCD_int
 * @remark
 * This will take \f$Θ(\log_{100}(x))\f$ time
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @see add_bcd
 */
BCD_int inc_bcd(const BCD_int x);

/**
 * @brief Subtract two @ref BCD_int "BCD_ints"
 * @param[in] x The left-hand @ref BCD_int
 * @param[in] y The right-hand @ref BCD_int
 * @return A @ref BCD_int reflecting the difference of the arguments
 * @memberof BCD_int
 * @remark
 * This will take \f$O(\log_{100}(max(x, y)))\f$ time
 * @note
 * Implementation details:
 * @note
 * This function works closely with @ref add_bcd.  If absolute value would go up from this operation, it is handed off to that method.
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 */
BCD_int sub_bcd(const BCD_int x, const BCD_int y);

/**
 * @brief Decrement a @ref BCD_int
 * @param[in] x The @ref BCD_int to decrement
 * @return A @ref BCD_int one less than x
 * @memberof BCD_int
 * @remark
 * This will take \f$Θ(\log_{100}(x))\f$ time
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @see sub_bcd
 */
BCD_int dec_bcd(const BCD_int x);

/**
 * @brief Multiply two @ref BCD_int "BCD_ints"
 * @param[in] x The left-hand @ref BCD_int
 * @param[in] y The right-hand @ref BCD_int
 * @return A @ref BCD_int reflecting the product of the arguments
 * @memberof BCD_int
 * @remark
 * This will take \f$Θ(\log_{100}(x) \cdot \log_{100}(y) \cdot \log{100}(xy))\f$ time if x and y are neither
 * @ref NaN "BCD_nan" nor zero. Otherwise it takes \f$Θ(1)\f$ time. For most purposes this can be simplified to
 * \f$O(\log_{100}^3(max(x, y)))\f$.
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 */
BCD_int mul_bcd(const BCD_int x, const BCD_int y);

/**
 * @brief Divide two @ref BCD_int "BCD_ints"
 * @param[in] x The left-hand @ref BCD_int
 * @param[in] y The right-hand @ref BCD_int
 * @return A @ref BCD_int reflecting the quotient of the arguments
 * @memberof BCD_int
 * @remark
 * This function has three behaviors for performance analysis
 * @remark
 * @li If x or y is @ref BCD_nan "NaN", or y is 0, then it will take \f$Θ(1)\f$ time
 * @li If y is 1, then it will take \f$Θ(\log_{100}(x))\f$ time
 * @li Otherwise it will take \f$O(\lceil\frac{x}{y}\rceil \cdot \log_{100}(x))\f$ time
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @attention
 * Returns @ref BCD_nan "NaN" with error set to @ref DIV_ZERO if y is 0
 */
BCD_int div_bcd(const BCD_int x, const BCD_int y);

/**
 * @brief Modulo divide two @ref BCD_int "BCD_ints"
 * @param[in] x The left-hand @ref BCD_int
 * @param[in] y The right-hand @ref BCD_int
 * @return A @ref BCD_int reflecting the remainder of the arguments
 * @memberof BCD_int
 * @remark
 * This function has two behaviors for performance analysis
 * @remark
 * @li If x or y is @ref BCD_nan "NaN", or y is either 0 or 1, then it will take \f$Θ(1)\f$ time
 * @li Otherwise it will take \f$O(\lceil\frac{x}{y}\rceil \cdot \log_{100}(x))\f$ time
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @attention
 * Returns @ref BCD_nan "NaN" with error set to @ref DIV_ZERO if y is 0
 */
BCD_int mod_bcd(const BCD_int x, const BCD_int y);

/**
 * @brief Divide and modulo divide two @ref BCD_int "BCD_ints"
 * @param[in] x The left-hand @ref BCD_int
 * @param[in] y The right-hand @ref BCD_int
 * @param[out] mod A pointer to the @ref BCD_int you would like to store the remainder
 * @return A @ref BCD_int reflecting the quotient of the arguments
 * @memberof BCD_int
 * @remark
 * This function has three behaviors for performance analysis
 * @remark
 * @li If x or y is @ref BCD_nan "NaN", or y is 0, then it will take \f$Θ(1)\f$ time
 * @li If y is 1, then it will take \f$Θ(\log_{100}(x))\f$ time
 * @li Otherwise it will take \f$O(\lceil\frac{x}{y}\rceil \cdot \log_{100}(x))\f$ time
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @attention
 * Returns (and assigns to mod) @ref BCD_nan "NaN" with error set to @ref DIV_ZERO if y is 0
 */
BCD_int divmod_bcd(const BCD_int x, BCD_int y, BCD_int *mod);

/**
 * @brief Raise x to the power of y
 * @param[in] x The left-hand @ref BCD_int
 * @param[in] y The right-hand @ref BCD_int
 * @return A @ref BCD_int reflecting the yth power of x
 * @memberof BCD_int
 * @remark
 * This will take \f$Θ(y \cdot \log_{100}(x)^3)\f$ time if x and y are not @ref NaN "BCD_nan" and y is positive.
 * Otherwise it takes \f$Θ(1)\f$ time.
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @attention
 * Returns @ref BCD_nan "NaN" with error set to @ref POW_NEG if y < 0 or x = 0
 */
BCD_int pow_bcd(const BCD_int x, const BCD_int y);

/**
 * @brief Get the factorial of x
 * @param[in] x The @ref BCD_int in question
 * @return A @ref BCD_int reflecting the factorial of x
 * @memberof BCD_int
 * @remark
 * This will take \f$O(x \cdot \log_{100}(x!)^3)\f$ time if x is not @ref NaN "BCD_nan" and is non-negative.
 * Otherwise it takes \f$Θ(1)\f$ time.
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @attention
 * Returns @ref BCD_nan "NaN" with error set to @ref FACT_NEG if x < 0
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
 * @param[in,out] x The @ref BCD_int in question
 * @param[in] negative The sign you would like to apply
 * @memberof BCD_int
 * @remarks
 * This function runs takes \f$Θ(1)\f$ time
 * @attention
 * All in-place operators may "return" @ref BCD_nan "NaN" with error set to @ref NO_MEM (for sign modifiers this is only possible if x is a constant)
 * @attention
 * All in-place operators will "return" @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and on sign modifiers error is left untouched
 */
void isign_bcd(BCD_int *const x, const bool negative);

/**
 * @brief Get the absolute value of a @ref BCD_int without relying on internals
 * @param[in,out] x The @ref BCD_int in question
 * @memberof BCD_int
 * @remarks
 * This function runs takes \f$Θ(1)\f$ time
 * @attention
 * All in-place operators may "return" @ref BCD_nan "NaN" with error set to @ref NO_MEM (for sign modifiers this is only possible if x is a constant)
 * @attention
 * All in-place operators will "return" @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and on sign modifiers error is left untouched
 */
void iabs_bcd(BCD_int *const x);

/**
 * @brief Get the negative absolute value of a @ref BCD_int without relying on internals
 * @param[in,out] x The @ref BCD_int in question
 * @memberof BCD_int
 * @remarks
 * This function runs takes \f$Θ(1)\f$ time
 * @attention
 * All in-place operators may "return" @ref BCD_nan "NaN" with error set to @ref NO_MEM (for sign modifiers this is only possible if x is a constant)
 * @attention
 * All in-place operators will "return" @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and on sign modifiers error is left untouched
 */
void ineg_bcd(BCD_int *const x);

/**
 * @brief Get the opposite value of a @ref BCD_int without relying on internals
 * @param[in,out] x The @ref BCD_int in question
 * @memberof BCD_int
 * @remarks
 * This function runs takes \f$Θ(1)\f$ time
 * @attention
 * All in-place operators may "return" @ref BCD_nan "NaN" with error set to @ref NO_MEM (for sign modifiers this is only possible if x is a constant)
 * @attention
 * All in-place operators will "return" @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and on sign modifiers error is left untouched
 */
void iopp_bcd(BCD_int *const x);

/**
 * @brief Add two @ref BCD_int "BCD_ints" and assign the result to x
 * @param[in,out] x The left-hand @ref BCD_int
 * @param[in] y The right-hand @ref BCD_int
 * @memberof BCD_int
 * @see add_bcd
 * for performance analysis
 * @attention
 * All in-place operators may "return" @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All in-place operators will "return" @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 */
void iadd_bcd(BCD_int *const x, const BCD_int y);

/**
 * @brief Increment a @ref BCD_int and assign the result to x
 * @param[in,out] x The left-hand @ref BCD_int
 * @memberof BCD_int
 * @see inc_bcd
 * for performance analysis
 * @attention
 * All in-place operators may "return" @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All in-place operators will "return" @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 */
void iinc_bcd(BCD_int *const x);

/**
 * @brief Subtract two @ref BCD_int "BCD_ints" and assign the result to x
 * @param[in,out] x The left-hand @ref BCD_int
 * @param[in] y The right-hand @ref BCD_int
 * @memberof BCD_int
 * @see sub_bcd
 * for performance analysis
 * @attention
 * All in-place operators may "return" @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All in-place operators will "return" @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 */
void isub_bcd(BCD_int *const x, const BCD_int y);

/**
 * @brief Decrement a @ref BCD_int and assign the result to x
 * @param[in,out] x The left-hand @ref BCD_int
 * @memberof BCD_int
 * @see dec_bcd
 * for performance analysis
 * @attention
 * All in-place operators may "return" @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All in-place operators will "return" @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 */
void idec_bcd(BCD_int *const x);

/**
 * @brief Multiply two @ref BCD_int "BCD_ints" and assign the result to x
 * @param[in,out] x The left-hand @ref BCD_int
 * @param[in] y The right-hand @ref BCD_int
 * @memberof BCD_int
 * @see mul_bcd
 * for performance analysis
 * @attention
 * All in-place operators may "return" @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All in-place operators will "return" @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 */
void imul_bcd(BCD_int *const x, const BCD_int y);

/**
 * @brief Divide two @ref BCD_int "BCD_ints" and assign the result to x
 * @param[in,out] x The left-hand @ref BCD_int
 * @param[in] y The right-hand @ref BCD_int
 * @memberof BCD_int
 * @see div_bcd
 * for performance analysis
 * @attention
 * All in-place operators may "return" @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All in-place operators will "return" @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @attention
 * "Returns" @ref BCD_nan "NaN" with error set to @ref DIV_ZERO if y is 0
 */
void idiv_bcd(BCD_int *const x, const BCD_int y);

/**
 * @brief Modulo divide two @ref BCD_int "BCD_ints" and assign the result to x
 * @param[in,out] x The left-hand @ref BCD_int
 * @param[in] y The right-hand @ref BCD_int
 * @memberof BCD_int
 * @see mod_bcd
 * for performance analysis
 * @attention
 * All in-place operators may "return" @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All in-place operators will "return" @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @attention
 * "Returns" @ref BCD_nan "NaN" with error set to @ref DIV_ZERO if y is 0
 */
void imod_bcd(BCD_int *const x, const BCD_int y);

/**
 * @brief Divide two @ref BCD_int "BCD_ints" then assign the quotient to x and remainder to y
 * @param[in,out] x The left-hand @ref BCD_int
 * @param[in,out] y The right-hand @ref BCD_int
 * @memberof BCD_int
 * @see divmod_bcd
 * for performance analysis
 * @attention
 * All in-place operators may "return" @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All in-place operators will "return" @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @attention
 * "Returns" two @ref BCD_nan "BCD_nans" with error set to @ref DIV_ZERO if y is 0
 */
void idivmod_bcd(BCD_int *const x, BCD_int *const y);

/**
 * @brief Raise x to the power of y then assign the result to x
 * @param[in,out] x The left-hand @ref BCD_int
 * @param[in] y The right-hand @ref BCD_int
 * @memberof BCD_int
 * @see pow_bcd
 * for performance analysis
 * @attention
 * All in-place operators may "return" @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All in-place operators will "return" @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @attention
 * "Returns" @ref BCD_nan "NaN" with error set to @ref POW_NEG if y < 0
 */
void ipow_bcd(BCD_int *const x, const BCD_int y);

/**
 * @brief Get the factorial of x then assign the result to x
 * @param[in,out] x The @ref BCD_int in question
 * @memberof BCD_int
 * @see factorial_bcd
 * for performance analysis
 * @attention
 * All in-place operators may "return" @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All in-place operators will "return" @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @attention
 * "Returns" @ref BCD_nan "NaN" with error set to @ref FACT_NEG if x < 0
 */
void ifactorial_bcd(BCD_int *const x);

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
 * @param[in] x The @ref BCD_int in question
 * @return Either the value of x OR -1 if x is @ref BCD_nan "NaN" or |x| would overflow
 * @memberof BCD_int
 * @remark
 * While this function techincally takes \f$O(\log_{100}(x))\f$ time, x is limited on most systems to 64-bit
 * values, so this function essentially runs in constant time (\f$Θ(1)\f$).
 */
uintmax_t abs_bcd_cuint(const BCD_int x);

/**
 * @brief Attempt to get the C-style signed representation of x
 * @param[in] x The @ref BCD_int in question
 * @return Either the value of x OR INTMAX_MIN if x is @ref BCD_nan "NaN" or |x| would overflow
 * @memberof BCD_int
 * @remark
 * While this function techincally takes \f$O(\log_{100}(x))\f$ time, x is limited on most systems to 64-bit
 * values, so this function essentially runs in constant time (\f$Θ(1)\f$).
 */
intmax_t  val_bcd_cint(const BCD_int x);

/**
 * @brief compare a @ref BCD_int with a C-style signed integer
 * @param[in] x The @ref BCD_int in question
 * @param[in] y The signed integer you would like to compare with
 * @return A @ref comp_t indicating the difference between x and y
 * @memberof BCD_int
 * @remark
 * While this function techincally takes \f$O(\log_{100}(x))\f$ time, y is limited on most systems to 64-bit
 * values, so this function essentially runs in constant time (\f$Θ(1)\f$).
 * @note Returns @ref NO_COMP if x is @ref BCD_nan "NaN"
 * @see cmp_bcd
 */
comp_t cmp_bcd_cint(const BCD_int x, const intmax_t y);

/**
 * @brief compare a @ref BCD_int with a C-style unsigned integer
 * @param[in] x The @ref BCD_int in question
 * @param[in] y The signed integer you would like to compare with
 * @return A @ref comp_t indicating the difference between x and y
 * @memberof BCD_int
 * @remark
 * While this function techincally takes \f$O(\log_{100}(x))\f$ time, y is limited on most systems to 64-bit
 * values, so this function essentially runs in constant time (\f$Θ(1)\f$).
 * @note Returns @ref NO_COMP if x is @ref BCD_nan "NaN"
 * @see cmp_bcd
 */
comp_t cmp_bcd_cuint(const BCD_int x, const uintmax_t y);

/**
 * @brief Multiply a @ref BCD_int by a C-style signed integer
 * @param[in] x The @ref BCD_int in question
 * @param[in] y The C-style integer to multiply by
 * @return The product of x and y
 * @memberof BCD_int
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 */
BCD_int mul_bcd_cint(const BCD_int x, const intmax_t y);

/**
 * @brief Multiply a @ref BCD_int by a C-style unsigned integer
 * @param[in] x The @ref BCD_int in question
 * @param[in] y The C-style integer to multiply by
 * @return The product of x and y
 * @memberof BCD_int
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 */
BCD_int mul_bcd_cuint(const BCD_int x, uintmax_t y);

/**
 * @brief Multiply a @ref BCD_int by a power of ten
 * @param[in] x The @ref BCD_int you'd like to multiply
 * @param[in] tens The power of ten to multiply by (in the form of \f$(10^{\texttt{tens}})\f$)
 * @return The result of \f$(x \cdot 10^{\texttt{tens}})\f$
 * @memberof BCD_int
 * @remark
 * This function takes \f$Θ(\log_{100}(x))\f$ time
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 */
BCD_int mul_bcd_pow_10(const BCD_int x, const size_t tens);

/**
 * @brief Multiply a @ref BCD_int by a power of ten
 * @param[in] a The @ref BCD_int you'd like to multiply
 * @param[in] tens The power of ten to multiply by (in the form of \f$(10^{\texttt{tens}})\f$)
 * @return The result of \f$(x \cdot 10^{\texttt{tens}})\f$
 * @memberof BCD_int
 * @remark
 * This function takes \f$Θ(\log_{100}(x))\f$ time
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @note
 * This is just an alias for @ref mul_bcd_pow_10
 */
BCD_int shift_bcd_left(const BCD_int a, const size_t tens);

/**
 * @brief Divide a @ref BCD_int by a power of ten
 * @param[in] x The @ref BCD_int you'd like to divide
 * @param[in] tens The power of ten to divide by (in the form of \f$(10^{\texttt{tens}})\f$)
 * @return The result of \f$(x \div 10^{\texttt{tens}})\f$
 * @memberof BCD_int
 * @remark
 * This function takes \f$Θ(\log_{100}(x))\f$ time
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 */
BCD_int div_bcd_pow_10(const BCD_int x, const size_t tens);

/**
 * @brief Divide a @ref BCD_int by a power of ten
 * @param[in] a The @ref BCD_int you'd like to divide
 * @param[in] tens The power of ten to divide by (in the form of \f$(10^{\texttt{tens}})\f$)
 * @return The result of \f$(x \div 10^{\texttt{tens}})\f$
 * @memberof BCD_int
 * @remark
 * This function takes \f$Θ(\log_{100}(x))\f$ time
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @note
 * This is just an alias for @ref div_bcd_pow_10
 */
BCD_int shift_bcd_right(const BCD_int a, const size_t tens);

/**
 * @brief Raise a C-style integer to the power of another C-style signed integer
 * @param[in] x The base
 * @param[in] y The exponent
 * @return The yth power of x in the form of a @ref BCD_int
 * @memberof BCD_int
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 */
BCD_int pow_cint_cuint(const intmax_t x, uintmax_t y);

/**
 * @brief Raise a C-style integer to the power of another C-style unsigned integer
 * @param[in] x The base
 * @param[in] y The exponent
 * @return The yth power of x in the form of a @ref BCD_int
 * @memberof BCD_int
 * @attention
 * All operators that return a @ref BCD_int may return @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All operators that return a @ref BCD_int will return @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 */
BCD_int pow_cuint_cuint(const uintmax_t x, uintmax_t y);

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
 * @param[in,out] x The @ref BCD_int in question
 * @param[in] y The C-style integer to multiply by
 * @memberof BCD_int
 * @see mul_bcd_cuint
 * for performance analysis
 * @attention
 * All in-place operators may "return" @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All in-place operators will "return" @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 */
void imul_bcd_cint(BCD_int *const x, const intmax_t y);

/**
 * @brief Multiply in-place a @ref BCD_int by a C-style unsigned integer
 * @param[in,out] x The @ref BCD_int in question
 * @param[in] y The C-style integer to multiply by
 * @memberof BCD_int
 * @see mul_bcd_cuint
 * for performance analysis
 * @attention
 * All in-place operators may "return" @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All in-place operators will "return" @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 */
void imul_bcd_cuint(BCD_int *const x, const uintmax_t y);

/**
 * @brief Multiply in-place a @ref BCD_int by a power of ten
 * @param[in,out] x The @ref BCD_int in question
 * @param[in] tens The power of ten you'd like to multiply by (in the form of \f$(10^{\texttt{tens}})\f$)
 * @memberof BCD_int
 * @see mul_bcd_pow_10
 * for performance analysis
 * @attention
 * All in-place operators may "return" @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All in-place operators will "return" @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 */
void imul_bcd_pow_10(BCD_int *const x, const size_t tens);

/**
 * @brief Multiply in-place a @ref BCD_int by a power of ten
 * @param[in,out] a The @ref BCD_int in question
 * @param[in] tens The power of ten you'd like to multiply by (in the form of \f$(10^{\texttt{tens}})\f$)
 * @memberof BCD_int
 * @see mul_bcd_pow_10
 * for performance analysis
 * @attention
 * All in-place operators may "return" @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All in-place operators will "return" @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 */
void ishift_bcd_left(BCD_int *const a, const size_t tens);

/**
 * @brief Divide in-place a @ref BCD_int by a power of ten
 * @param[in,out] x The @ref BCD_int in question
 * @param[in] tens The power of ten you'd like to divide by (in the form of \f$(10^{\texttt{tens}})\f$)
 * @memberof BCD_int
 * @see div_bcd_pow_10
 * for performance analysis
 * @attention
 * All in-place operators may "return" @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All in-place operators will "return" @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @todo
 * This function is not yet supported for odd values of tens
 */
void idiv_bcd_pow_10(BCD_int *const x, const size_t tens);

/**
 * @brief Divide in-place a @ref BCD_int by a power of ten
 * @param[in,out] a The @ref BCD_int in question
 * @param[in] tens The power of ten you'd like to divide by (in the form of \f$(10^{\texttt{tens}})\f$)
 * @memberof BCD_int
 * @see div_bcd_pow_10
 * for performance analysis
 * @attention
 * All in-place operators may "return" @ref BCD_nan "NaN" with error set to @ref NO_MEM
 * @attention
 * All in-place operators will "return" @ref BCD_nan "NaN" if fed @ref BCD_nan "NaN", and set error to @ref BCD_error "{OP}_NAN".
 * @todo
 * This function is not yet supported for odd values of tens
 */
void ishift_bcd_right(BCD_int *const a, const size_t tens);

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
 * @param[in] ab The first byte, which gets split into nibbles a and b
 * @param[in] cd The second byte, which gets split into nibbles c and d
 * @remarks
 * This function runs takes \f$Θ(1)\f$ time
 * @returns \f$100 \cdot a \cdot c + 10 \cdot (a \cdot d + b \cdot c) + b \cdot d\f$
 */
uint16_t mul_dig_pair(const packed_BCD_pair ab, const packed_BCD_pair cd);

/**
 * @brief Print a @ref BCD_int to the screen
 * @param[in] x The @ref BCD_int you'd like to print
 * @memberof BCD_int
 * @remarks
 * This function runs takes \f$Θ(\log_{100}(x))\f$ time
 */
void print_bcd(const BCD_int x);

/**
 * @brief Print a @ref BCD_int to the screen and append a newline
 * @param[in] x The @ref BCD_int you'd like to print
 * @memberof BCD_int
 * @remarks
 * This function runs takes \f$Θ(\log_{100}(x))\f$ time
 */
void print_bcd_ln(const BCD_int x);

/** @} */ //
/** @} */ // end of bcd_utility

inline void free_BCD_int(BCD_int *const x)  {
    if (x->error != IS_FREED && !x->constant)   {
        free(x->data);
        *x = (BCD_int) {.nan = true, .error = IS_FREED, .orig_error = IS_FREED};
    }
}

BCD_int new_BCD_int1(const intmax_t a)  {
    if (a < 0)
        return new_BCD_int2((uintmax_t) (-a), true);
    return new_BCD_int2((uintmax_t) a, false);
}

BCD_int new_BCD_int2(uintmax_t a, const bool negative)  {
    BCD_int c = {
        .negative = negative,
        .even = !(a % 2),
        .zero = !a,
        .decimal_digits = (size_t) ceil(log10((double) (a + 1))),
    };
    c.bcd_digits = (c.decimal_digits + 1) / 2;
    c.data = (packed_BCD_pair *) malloc(sizeof(packed_BCD_pair) * c.bcd_digits);
    if (unlikely(c.data == NULL))
        return bcd_error(NO_MEM, NO_MEM);
    for (size_t i = 0; i < c.bcd_digits; i++)   {
        c.data[i] = (((a % 100) / 10) << 4) | (a % 10);
        a /= 100;
    }
    return c;
}

inline BCD_int copy_BCD_int(BCD_int a)  {
    if (likely(a.data != NULL))   {
        const packed_BCD_pair *const data = a.data;
        a.data = (packed_BCD_pair *) malloc(sizeof(packed_BCD_pair) * a.bcd_digits);
        if (unlikely(a.data == NULL))
            return bcd_error(NO_MEM, NO_MEM);
        memcpy(a.data, data, a.bcd_digits);
    }
    a.constant = false;
    return a;
}

BCD_int BCD_from_bytes(const unsigned char *const str, const size_t chars, const bool negative, const bool little_endian)   {
    // converts a BCD-formatted bytestring to a little-endian BCD int
    if (!chars || str == NULL)
        return BCD_zero;
    size_t i;
    BCD_int c = (BCD_int) {
        .negative = negative,
        .data = (packed_BCD_pair *) malloc(sizeof(packed_BCD_pair) * chars),
    };
    if (unlikely(c.data == NULL))
        return bcd_error(NO_MEM, NO_MEM);
    if (little_endian)  {
        memcpy(c.data, str, chars);
    }
    else    {
        size_t j;
        for (i = 0, j = chars - 1; i < chars; i++, j--) {
            c.data[i] = str[j];
        }
    }
    for (i = chars - 1; i != -1; i--)   {
        if (c.data[i])    {
            c.bcd_digits = i + 1;
            if (c.data[i] & 0xF0)
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
    c.even = !(c.data[0] % 2);
    return c;
}

BCD_int BCD_from_ascii(const char *const str, const size_t digits, const bool negative) {
    // packs an ASCII digit string into big-endian bytes, then runs through BCD_from_bytes()
    const size_t length = (digits + 1) / 2;
    size_t i, j;
    unsigned char * const bytes = (unsigned char *) malloc(sizeof(unsigned char) * length);
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
    return (BCD_int) {  // note that uninitialized data is 0 or NULL
        .nan = true,
        .error = error,
        .orig_error = orig_error
    };
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
        if (x.data[i] != y.data[i]) {
            if (x.negative)
                return (x.data[i] > y.data[i]) ? LESS_THAN : GREATER_THAN;
            return (x.data[i] > y.data[i]) ? GREATER_THAN : LESS_THAN;
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
    if (likely(!x.nan))
        x.negative = negative;
    return (no_copy) ? x : copy_BCD_int(x);
}

inline BCD_int abs_bcd(const BCD_int x, const bool no_copy) {
    return sign_bcd(x, no_copy, false);
}

inline BCD_int neg_bcd(const BCD_int x, const bool no_copy) {
    return sign_bcd(x, no_copy, true);
}

inline BCD_int opp_bcd(const BCD_int x, const bool no_copy) {
    return sign_bcd(x, no_copy, !x.negative);
}

BCD_int add_bcd(const BCD_int x, const BCD_int y)   {
    if (unlikely(x.nan || y.nan))
        return bcd_error(ADD_NAN, (x.nan) ? x.orig_error : y.orig_error);
    if (unlikely(x.zero))
        return copy_BCD_int(y);
    if (unlikely(y.zero))
        return copy_BCD_int(x);
    if (x.negative != y.negative)
        return sub_bcd(x, opp_bcd(y, true));
        // if signs don't match, absolute value would go down.
        // that means we need to flip y's sign and move through sub_bcd()
    if (cmp_bcd(abs_bcd(x, true), abs_bcd(y, true)) == LESS_THAN)  // remember this compares absolute values
        return add_bcd(y, x);
        // this lets us have x be constant
    BCD_int z = (BCD_int) {
        .negative = x.negative,  // we know this is also y.negative
        .data = (packed_BCD_pair *) malloc(sizeof(packed_BCD_pair) * (x.bcd_digits + 1)),  // in case of final overflow
    };
    if (unlikely(z.data == NULL))
        return bcd_error(NO_MEM, NO_MEM);
    size_t i;
    uint8_t overflow = false;  // note that it's only ever set to 0 or 1, but assembly complains if bool is specified
    packed_BCD_pair a, b, c;
    for (i = 0; i < y.bcd_digits; i++)  {
        a = x.data[i];
        b = y.data[i];
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
        z.data[i] = c;
    }
    for (; i < x.bcd_digits; i++)   {  // while there's overflow and digits, continue adding
        a = x.data[i] + overflow;
        if ((a & 0x0F) == 0x0A)  // since all that's left is overflow, we don't need to check ranges
            a += 0x06;
        if ((overflow = ((a & 0xF0) == 0xA0)))
            a += 0x60;
        z.data[i] = a;
    }
    if (i < x.bcd_digits)  // if there's no more overflow, but still digits left, copy directly
        memcpy(z.data + i, x.data + i, x.bcd_digits - i);
    z.bcd_digits = x.bcd_digits + overflow;
    if ((z.data[x.bcd_digits] = overflow))
        z.decimal_digits = x.bcd_digits * 2 + 1;
    else if (z.data[x.bcd_digits - 1] & 0xF0)
        z.decimal_digits = x.bcd_digits * 2;
    else
        z.decimal_digits = x.bcd_digits * 2 - 1;
    z.even = !(z.data[0] % 2);
    return z;
}

inline BCD_int inc_bcd(const BCD_int x) {
    return add_bcd(x, BCD_one);
}

BCD_int sub_bcd(const BCD_int x, const BCD_int y)   {
    if (x.negative != y.negative)
        return add_bcd(x, opp_bcd(y, true));
        // if signs don't match, absolute value would go up.
        // that means we need to flip y's sign and move through add_bcd()
    if (unlikely(x.nan || y.nan))
        return bcd_error(SUB_NAN, (x.nan) ? x.orig_error : y.orig_error);
    const comp_t cmp = cmp_bcd(x, y);
    if (unlikely(cmp == EQUAL_TO))
        return BCD_zero;
    if (unlikely(x.zero))
        return opp_bcd(y, false);
    if (unlikely(y.zero))
        return copy_BCD_int(x);
    if ((x.negative && cmp == GREATER_THAN) || (!x.negative && cmp == LESS_THAN))
        return opp_bcd(sub_bcd(y, x), true);
        // it's easier to do this than it is to properly handle 10s complement math
    BCD_int z = (BCD_int) {
        .data = (packed_BCD_pair *) malloc(sizeof(packed_BCD_pair) * x.bcd_digits),
        .negative = x.negative,  // remember this is the same as y.negative, and that |y| is less than |x|
    };
    if (unlikely(z.data == NULL))
        return bcd_error(NO_MEM, NO_MEM);
    bool carry = false;
    size_t i;
    packed_BCD_pair a, b, c;
    for (i = 0; i < y.bcd_digits; i++)  {
        a = x.data[i];
        b = y.data[i];
        b += carry;  // incorporate carry from last pair
        #if (!defined(NO_ASSEMBLY) && X86_COMPILER)  // if on these architectures, there's assembly tricks to adjust BCD in-silicon
            #if CL_COMPILER
                // CL compiler has a different syntax for inline assembly, does a lot less lifting
                // note that the syntax here is: op [dest [, src]]
                // brackets indicate a C symbol is referenced rather than a register
                __asm   {
                    mov al, [a];         // move C symbol a to register al
                    sub al, [b];         // subtract C symbol b from register al
                    das;                 // have the CPU make sure register al contains valid, packed BCD digits
                    setc [carry];        // set C symbol carry to contain the carry bit, set by daa
                    mov [c], al;         // move register al to C symbol c
                }
            #else
                // this is the standard GCC/LLVM syntax for it
                // note that the syntax here is: op [[src, ] dest]
                // %\d indicates a C symbol is referenced, see the lookups at the end of code for which
                __asm__(
                    "sub %3, %%al;"  // subtract the register containing b from al
                    "das;"           // have the CPU make sure register al contains valid, packed BCD digits
                    "setc %1;"       // set the register containing carry to hold the carry bit, set by daa
                                     // this next section tells the compiler what to do after execution
                    : "=a" (c),      // store the contents of register al in symbol c
                    "=rgm" (carry)   // and a general register or memory location gets assigned to symbol carry (referenced as %1)
                                     // then below tells the compiler what our inputs are
                    : "a" (a),       // symbol a should get dumped to register al
                    "rgm" (b)        // and symbol b in a general register or memory location (referenced as %3)
                );
            #endif
        #else  // otherwise fall back to doing it in C. the code below should replicate the DAS x86 instruction
            const uint8_t old_c = c = a - b;
            const bool AF = ((int8_t) (a & 0x0F) - (b & 0x0F)) < 0;  // this is to replicate the x86 AF flag
            const bool old_carry = ((int16_t) a - b) < 0;
            carry = false;
            if (AF || ((c & 0x0F) > 0x09))  {  // if the lower nibble borrowed, or it's greater than 9
                carry = old_carry || (((int16_t) c - 0x06) < 0);
                c -= 0x06;
            }
            if (old_carry || (old_c > 0x99))    {  // if the initial subtraction carried, or the original value overflowed
                carry = true;
                c -= 0x60;
            }
        #endif
        z.data[i] = c;
    }
    for (; carry && i < x.bcd_digits; i++)  {  // while there's carry and digits, continue adding
        a = x.data[i] - carry;
        if ((a & 0x0F) == 0x0F)  // since all that's left is carry, we don't need to check ranges
            a -= 0x06;
        if ((carry = ((a & 0xF0) == 0xF0)))
            a -= 0x60;
        z.data[i] = a;
    }
    if (i < x.bcd_digits)  // if there's no more carry, but still digits left, copy directly
        memcpy(z.data + i, x.data + i, x.bcd_digits - i);
    i = x.bcd_digits;
    while (--i != -1)   {
        if (z.data[i])    {
            z.bcd_digits = i + 1;
            if (z.data[i] & 0xF0)
                z.decimal_digits = 2 * z.bcd_digits;
            else
                z.decimal_digits = 2 * z.bcd_digits - 1;
            z.even = !(z.data[0] % 2);
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
    size_t i, j, pow_10, ipow_10 = 0;
    uint16_t staging;
    for (i = 0; i < x.bcd_digits; i++, ipow_10 += 2)  {
        for (j = 0, pow_10 = ipow_10; j < y.bcd_digits; j++, pow_10 += 2) {
            if ((staging = mul_dig_pair(x.data[i], y.data[j])) == 0)
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
    return divmod_bcd(x, y, NULL);
}

inline BCD_int mod_bcd(const BCD_int x, const BCD_int y)    {
    if (y.decimal_digits == 1 && y.data[0] == 1)  {
        return BCD_zero;
    }
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
        div = bcd_error(error, orig_error);
        if (mod != NULL)
            *mod = div;
        return div;
    }
    if (unlikely(x.zero))   {
        if (mod != NULL)
            *mod = BCD_zero;
        return div;
    }
    if (y.decimal_digits == 1 && y.data[0] == 1)  {
        if (mod != NULL)
            *mod = BCD_zero;
        return sign_bcd(copy_BCD_int(x), true, x.negative != y.negative);
    }
    const bool x_negative = x.negative, y_negative = y.negative;
    const BCD_int divisor = abs_bcd(y, true);
    BCD_int tmp = abs_bcd(x, false);
    while ((tmp.decimal_digits - 1) > divisor.decimal_digits)   {  // first go through it by multiples of 10
        const size_t pow_10 = tmp.decimal_digits - divisor.decimal_digits - 1;
        BCD_int tmp2 = mul_bcd_pow_10(divisor, pow_10);
        isub_bcd(&tmp, tmp2);
        free_BCD_int(&tmp2);
        tmp2 = mul_bcd_pow_10(BCD_one, pow_10);
        iadd_bcd(&div, tmp2);
        free_BCD_int(&tmp2);
    }
    while (cmp_bcd(tmp, divisor) != LESS_THAN)  {  // x and y can't be NaN, so this is safe
        isub_bcd(&tmp, divisor);
        iinc_bcd(&div);
    }
    if ((div.negative = (x_negative != y_negative)))    {
        if (!tmp.zero)
            idec_bcd(&div);
        if (mod != NULL)
            *mod = sign_bcd(sub_bcd(divisor, tmp), true, y_negative);
        free_BCD_int(&tmp);
    }
    else if (mod != NULL) {
        *mod = sign_bcd(tmp, true, y_negative);
    }
    else   {
        free_BCD_int(&tmp);
    }
    return div;
}

BCD_int pow_bcd(const BCD_int x, const BCD_int y)   {
    // this takes O(y * 2log_100(x)^3) time
    if (unlikely(x.nan || y.nan || y.negative || x.zero))   {
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
    if (unlikely(x.zero || (x.decimal_digits == 1 && x.data[0] == 1)))
        return BCD_one;
    BCD_int i = dec_bcd(x), ret = copy_BCD_int(x);
    while (!i.zero) {
        imul_bcd(&ret, i);
        idec_bcd(&i);
    }
    free_BCD_int(&i);
    return ret;
}

inline void isign_bcd(BCD_int *const x, const bool negative)    {
    if (!x->nan && x->constant)
        *x = copy_BCD_int(*x);
    if (likely(!x->nan))  // this needs a second check because copy can return NaN
        x->negative = negative;
}

inline void iabs_bcd(BCD_int *const x)  {
    isign_bcd(x, false);
}

inline void ineg_bcd(BCD_int *const x)  {
    isign_bcd(x, true);
}

inline void iopp_bcd(BCD_int *const x)  {
    isign_bcd(x, !x->negative);
}

inline void iadd_bcd(BCD_int *const x, const BCD_int y) {
    BCD_int ret = add_bcd(*x, y);
    free_BCD_int(x);
    *x = ret;
}

inline void iinc_bcd(BCD_int *const x)  {
    BCD_int ret = inc_bcd(*x);
    free_BCD_int(x);
    *x = ret;
}

inline void isub_bcd(BCD_int *const x, const BCD_int y) {
    BCD_int ret = sub_bcd(*x, y);
    free_BCD_int(x);
    *x = ret;
}

inline void idec_bcd(BCD_int *const x)  {
    BCD_int ret = dec_bcd(*x);
    free_BCD_int(x);
    *x = ret;
}

inline void imul_bcd(BCD_int *const x, const BCD_int y) {
    BCD_int ret = mul_bcd(*x, y);
    free_BCD_int(x);
    *x = ret;
}

inline void idiv_bcd(BCD_int *const x, const BCD_int y) {
    BCD_int ret = div_bcd(*x, y);
    free_BCD_int(x);
    *x = ret;
}

inline void imod_bcd(BCD_int *const x, const BCD_int y) {
    BCD_int ret = mod_bcd(*x, y);
    free_BCD_int(x);
    *x = ret;
}

inline void idivmod_bcd(BCD_int *const x, BCD_int *const y) {
    BCD_int mod, div = divmod_bcd(*x, *y, &mod);
    free_BCD_int(x);
    free_BCD_int(y);
    *x = div;
    *y = mod;
}

inline void ipow_bcd(BCD_int *const x, const BCD_int y) {
    BCD_int ret = pow_bcd(*x, y);
    free_BCD_int(x);
    *x = ret;
}

void ifactorial_bcd(BCD_int *const x)    {
    BCD_int ret = factorial_bcd(*x);
    free_BCD_int(x);
    *x = ret;
}

uintmax_t abs_bcd_cuint(const BCD_int x)    {
    if (unlikely(x.nan || (x.decimal_digits > POW_OF_MAX_POW_10_64 + 2)))
        return -1;  // overflow certain, or invalid number
    uintmax_t answer = 0, pow_10 = 1;
    for (size_t i = 0; i < x.bcd_digits; i++, pow_10 *= 100) {
        answer += pow_10 * ((x.data[i] & 0x0F) + 10 * (x.data[i] >> 4));
    }
    if ((x.data[0] % 0xF) != (answer % 10))
        return -1;  // overflowed
    return answer;
}

intmax_t val_bcd_cint(const BCD_int x)  {
    if (unlikely(x.nan || (x.decimal_digits > POW_OF_MAX_POW_10_64 + 1)))
        return INTMAX_MIN;  // overflow certain, or invalid number
    uintmax_t answer = abs_bcd_cuint(x);
    if (answer > (uintmax_t) INTMAX_MAX)
        return INTMAX_MIN;  // overflowed
    return  ((x.negative) ? -((intmax_t) answer) : (intmax_t) answer);
}

inline comp_t cmp_bcd_cint(const BCD_int x, const intmax_t y)   {
    if (y < 0)  {
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
    uint8_t tens = 0;  // will up size when there's an 848-bit system
    BCD_int ret = BCD_zero, mul_by_power_10;
    while (y % 10 == 0) {
        y /= 10;  // first remove factors of ten
        ++tens;
    }
    if (tens)
        ret = mul_bcd_pow_10(x, tens);
    // then for decreasing powers of ten, batch additions
    tens = POW_OF_MAX_POW_10_64;
    for (uintmax_t p = MAX_POW_10_64; p > 1; p /= 10, --tens)   {
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

BCD_int mul_bcd_pow_10(const BCD_int x, const size_t tens)  {
    // this takes O(log_100(x)) time. Note that it's significantly faster if tens is even
    // returns x * 10^tens
    if (unlikely(x.nan))
        return bcd_error(SHIFT_NAN, x.orig_error);
    if (unlikely(x.zero))
        return BCD_zero;
    if (unlikely(!tens))
        return copy_BCD_int(x);
    BCD_int ret = {
        .even = x.even || !!tens,
        .negative = x.negative,
        .decimal_digits = (size_t) x.decimal_digits + tens,
    };
    ret.bcd_digits = (ret.decimal_digits + 1) / 2;
    ret.data = (packed_BCD_pair *) calloc(ret.bcd_digits, sizeof(packed_BCD_pair));
    if (unlikely(ret.data == NULL))
        return bcd_error(NO_MEM, NO_MEM);
    if (tens % 2 == 0)  {
        // +--+--+    +--+--+--+
        // |23|01| -> ...|23|01|
        // +--+--+    +--+--+--+
        const size_t digit_diff = ret.bcd_digits - x.bcd_digits;
        memcpy(ret.data + digit_diff, x.data, x.bcd_digits);
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
        ret.data[digit_diff] = x.data[0] << 4;
        for (size_t i = 1; i < x.bcd_digits; i++)   {
            ret.data[i + digit_diff] = x.data[i] << 4;
            ret.data[i + digit_diff] |= x.data[i - 1] >> 4;
        }
        if (ret.decimal_digits % 2)
            ret.data[x.bcd_digits + digit_diff] |= x.data[x.bcd_digits - 1] >> 4;
    }
    return ret;
}

inline BCD_int shift_bcd_left(const BCD_int x, const size_t tens)    {
    return mul_bcd_pow_10(x, tens);
}

BCD_int div_bcd_pow_10(const BCD_int a, const size_t tens)   {
    if (unlikely(a.nan))
        return bcd_error(SHIFT_NAN, a.orig_error);
    if (unlikely(!tens))
        return copy_BCD_int(a);
    if (tens >= a.decimal_digits)
        return BCD_zero;
    BCD_int ret = (BCD_int) {
        .decimal_digits = a.decimal_digits - tens
    };
    ret.bcd_digits = (ret.decimal_digits + 1) / 2;
    ret.data = (packed_BCD_pair *) malloc(sizeof(packed_BCD_pair) * ret.bcd_digits);
    if (unlikely(ret.data == NULL))
        return bcd_error(NO_MEM, NO_MEM);
    const size_t digit_diff = a.bcd_digits - ret.bcd_digits;
    if (tens % 2 == 0)  {
        // +--+--+--+    +--+--+
        // ...|23|01| -> |23|01|
        // +--+--+--+    +--+--+
        memcpy(ret.data, a.data + digit_diff, ret.bcd_digits);
    }
    else    {
        // +--+--+--+--+    +--+--+
        // ...|45|23|01| -> |34|12|
        // +--+--+--+--+    +--+--+
        // +--+--+--+--+    +--+--+--+
        // ...|56|34|12| -> |45|23|01|
        // +--+--+--+--+    +--+--+--+
        ret.data[0] = a.data[digit_diff] >> 4;
        for (size_t i = 1; i < ret.bcd_digits; i++) {
            ret.data[i - 1] |= a.data[i + digit_diff] << 4;
            ret.data[i] = a.data[i + digit_diff] >> 4;
        }
        if (a.decimal_digits % 2)
            ret.data[ret.bcd_digits - 1] |= a.data[ret.bcd_digits + digit_diff] << 4;
    }
    ret.even = !(ret.data[0] % 2);
    return ret;
}

inline BCD_int shift_bcd_right(const BCD_int a, const size_t tens)   {
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

inline void imul_bcd_cuint(BCD_int *const x, const uintmax_t y) {
    BCD_int ret = mul_bcd_cuint(*x, y);
    free_BCD_int(x);
    *x = ret;
}

inline void imul_bcd_cint(BCD_int *const x, const intmax_t y)   {
    BCD_int ret = mul_bcd_cint(*x, y);
    free_BCD_int(x);
    *x = ret;
}

inline void imul_bcd_pow_10(BCD_int *const x, const size_t tens) {
    BCD_int ret = mul_bcd_pow_10(*x, tens);
    free_BCD_int(x);
    *x = ret;
}

inline void ishift_bcd_left(BCD_int *const x, const size_t tens) {
    imul_bcd_pow_10(x, tens);
}

inline void idiv_bcd_pow_10(BCD_int *const a, const size_t tens) {
    BCD_int ret = div_bcd_pow_10(*a, tens);
    free_BCD_int(a);
    *a = ret;
}

inline void ishift_bcd_right(BCD_int *const a, const size_t tens)    {
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
    printf("%x", x.data[i--]);
    for (; i != -1; i--)    {
        printf("%02x", x.data[i]);
    }
}

inline void print_bcd_ln(const BCD_int x)   {
    print_bcd(x);
    printf("\n");
}

#ifdef DOXYGEN
};
#endif
