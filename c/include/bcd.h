#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "macros.h"

#if !PCC_COMPILER
    #include <stdlib.h>
    #include <math.h>
#else
    #include "math.h"
#endif

EXTERN_PRINTF;

typedef unsigned char packed_BCD_pair;

typedef enum comp_t {
    EQUAL_TO,      // x == y
    GREATER_THAN,  // x > y
    LESS_THAN,     // x < y
    NO_COMP        // x.nan || y.nan
} comp_t;

typedef enum BCD_error  {
    NON_ERR,    // there wasn't an error
    ORIG_NAN,   // this is the original NaN
    IS_FREED,   // this BCD_int has been freed
    ADD_NAN,    // you tried to add NaN
    SUB_NAN,    // you tried to subtract NaN
    MUL_NAN,    // you tried to multiply NaN
    DIV_NAN,    // you tried to divide NaN
    POW_NAN,    // you tried to use NaN as an exponent or base
    FACT_NAN,   // you tried to get the factorial of NaN
    SHIFT_NAN,  // you tried to shift NaN
    POW_NEG,    // you tried to use a negative power
    FACT_NEG,   // you tried to get a negative factorial
    DIV_ZERO,   // you tried to divide by zero
    // 0b1101   // reserved
    // 0b1110   // reserved
    // 0b1111   // reserved
} BCD_error;

typedef struct {
    // a little-endian, arbitrary-precision, binary-coded decimal number
    packed_BCD_pair *digits;  // the raw digits of the integer, DO NOT modify directly
    size_t bcd_digits;        // the byte count of digits
    size_t decimal_digits;    // the number of decimal digits in this integer
    bool negative : 1;        // indicates the integer is negative
    bool zero : 1;            // indicates the integer is 0
    bool even : 1;            // indicates the integer is even
    bool constant : 1;        // indicates that the integer is a constant and will not be touched by free_bcd_int()
    union {
        char nan : 4;         // indicates that the integer is NaN
        BCD_error error : 4;  // this specifies why the BCD_int is NaN, if it is
    };
} BCD_int;

// constants
const BCD_int BCD_one  = {(packed_BCD_pair *) "\x01", 1, 1, false, false, false, true, {NON_ERR}};
const BCD_int BCD_zero = {(packed_BCD_pair *) NULL,   0, 0, false, true,  true,  true, {NON_ERR}};
const BCD_int BCD_nan  = {(packed_BCD_pair *) NULL,   0, 0, false, false, false, true, {ORIG_NAN}};

// destructor
void free_BCD_int(BCD_int *x);  // if not constant, sets all flags to 0, error to IS_FREED

// constructors
BCD_int new_BCD_int1  (intmax_t a);
BCD_int new_BCD_int2  (uintmax_t a,                    bool negative);
BCD_int copy_BCD_int  (BCD_int a);
BCD_int BCD_from_bytes(const unsigned char *const str, size_t chars,  bool negative, bool little_endian);
BCD_int BCD_from_ascii(const char *const str,          size_t digits, bool negative);

// operators
// note: all operators will return BCD_nan if fed BCD_nan, and set error to {OP}_NAN
comp_t  cmp_bcd      (BCD_int x, BCD_int y);                // returns NO_COMP if either is NaN
BCD_int add_bcd      (BCD_int x, BCD_int y);
BCD_int inc_bcd      (BCD_int x);
BCD_int sub_bcd      (BCD_int x, BCD_int y);
BCD_int dec_bcd      (BCD_int x);
BCD_int mul_bcd      (BCD_int x, BCD_int y);
BCD_int div_bcd      (BCD_int x, BCD_int y);                // sets error to DIV_ZERO if y is 0
BCD_int mod_bcd      (BCD_int x, BCD_int y);                // sets error to DIV_ZERO if y is 0
BCD_int divmod_bcd   (BCD_int x, BCD_int y, BCD_int *mod);  // sets error to DIV_ZERO if y is 0
BCD_int pow_bcd      (BCD_int x, BCD_int y);                // sets error to POW_NEG if y is negative
BCD_int factorial_bcd(BCD_int x);                           // sets error to FACT_NEG if x is negative

// in-place operators
// note: all operators will return BCD_nan if fed BCD_nan, and set error to {OP}_NAN
void iadd_bcd      (BCD_int *x, BCD_int y);
void iinc_bcd      (BCD_int *x);
void isub_bcd      (BCD_int *x, BCD_int y);
void idec_bcd      (BCD_int *x);
void imul_bcd      (BCD_int *x, BCD_int y);
void idiv_bcd      (BCD_int *x, BCD_int y);   // sets error to DIV_ZERO if y is 0
void imod_bcd      (BCD_int *x, BCD_int y);   // sets error to DIV_ZERO if y is 0
void idivmod_bcd   (BCD_int *x, BCD_int *y);  // sets error to DIV_ZERO if y is 0
void ipow_bcd      (BCD_int *x, BCD_int y);   // sets error to POW_NEG if y is negative
void ifactorial_bcd(BCD_int *x);              // sets error to FACT_NEG if x is negative

// operators with C integers
// note: all operators will return BCD_nan if fed BCD_nan, and set error to {OP}_NAN
BCD_int mul_bcd_cint   (BCD_int x,   intmax_t y);
BCD_int mul_bcd_cuint  (BCD_int x,   uintmax_t y);
BCD_int mul_bcd_pow_10 (BCD_int x,   uintmax_t tens);
BCD_int shift_bcd_left (BCD_int a,   uintmax_t tens);
BCD_int div_bcd_pow_10 (BCD_int x,   uintmax_t tens);  // not yet supported for odd tens
BCD_int shift_bcd_right(BCD_int a,   uintmax_t tens);  // not yet supported for odd tens
BCD_int pow_cint_cuint (intmax_t x,  uintmax_t y);
BCD_int pow_cuint_cuint(uintmax_t x, uintmax_t y);

// in-place operators with C integers
// note: all operators will return BCD_nan if fed BCD_nan, and set error to {OP}_NAN
void imul_bcd_cint   (BCD_int *x, intmax_t y);
void imul_bcd_cuint  (BCD_int *x, uintmax_t y);
void imul_bcd_pow_10 (BCD_int *x, uintmax_t tens);
void ishift_bcd_left (BCD_int *a, uintmax_t tens);
void idiv_bcd_pow_10 (BCD_int *x, uintmax_t tens);  // not yet supported for odd tens
void ishift_bcd_right(BCD_int *a, uintmax_t tens);  // not yet supported for odd tens

// utility methods
void     print_bcd   (BCD_int x);
void     print_bcd_ln(BCD_int x);
uint16_t mul_dig_pair(packed_BCD_pair ab, packed_BCD_pair cd);

inline void free_BCD_int(BCD_int *x)    {
    if (x->error != IS_FREED && !x->constant)   {
        free(x->digits);
        x->digits = NULL;
        x->error = IS_FREED;
        x->zero = x->even = x->negative = false;
    }
}

BCD_int new_BCD_int1(intmax_t a)    {
    if (a < 0)  {
        return new_BCD_int2((uintmax_t) (-a), true);
    }
    return new_BCD_int2((uintmax_t) a, false);
}

BCD_int new_BCD_int2(uintmax_t a, bool negative)   {
    BCD_int c;
    #if !PCC_COMPILER
        c.decimal_digits = ceil(log10(a + 1));
    #else
        c.decimal_digits = imprecise_log10(a + 1);
    #endif
    c.bcd_digits = (c.decimal_digits + 1) / 2;
    c.digits = (packed_BCD_pair *) malloc(sizeof(packed_BCD_pair) * c.bcd_digits);
    c.negative = negative;
    c.zero = !a;
    c.even = !(a % 2);
    c.nan = false;
    c.constant = false;
    c.error = NON_ERR;
    for (size_t i = 0; i < c.bcd_digits; i++)   {
        c.digits[i] = (((a % 100) / 10) << 4) | (a % 10);
        a /= 100;
    }
    return c;
}

inline BCD_int copy_BCD_int(BCD_int a)  {
    packed_BCD_pair *digits = a.digits;
    a.digits = (packed_BCD_pair *) malloc(sizeof(packed_BCD_pair) * a.bcd_digits);
    memcpy(a.digits, digits, a.bcd_digits);
    return a;
}

BCD_int BCD_from_bytes(const unsigned char *const str, size_t chars, bool negative, bool little_endian) {
    // converts a BCD-formatted bytestring to a little-endian BCD int
    if (!chars || str == NULL)  {
        return BCD_zero;
    }
    BCD_int c;
    size_t i;
    c.zero = c.constant = false;
    c.nan = false;
    c.negative = negative;
    c.digits = (packed_BCD_pair *) malloc(sizeof(packed_BCD_pair) * chars);
    if (little_endian)  {
        for (i = 0; i < chars; i++) {
            c.digits[i] = str[i];
        }
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
            if (c.digits[i] & 0xF0) {
                c.decimal_digits = i * 2 + 1;
            }
            else    {
                c.decimal_digits = i * 2;
            }
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

BCD_int BCD_from_ascii(const char *const str, size_t digits, bool negative) {
    // packs an ASCII digit string into big-endian bytes, then runs through BCD_from_bytes()
    const size_t length = (digits + 1) / 2;
    size_t i, j;
    unsigned char *bytes = (unsigned char *) malloc(sizeof(unsigned char) * length);
    j = i = digits % 2;
    if (i)  {
        bytes[0] = str[0] - '0';
    }
    for (; i < length; i++, j += 2) {
        bytes[i] = ((str[j] - '0') << 4) | ((str[j + 1] - '0') & 0xF);
    }
    BCD_int ret = BCD_from_bytes(bytes, length, negative, false);
    free(bytes);
    return ret;
}

comp_t cmp_bcd(BCD_int x, BCD_int y)    {
    // returns:
    // NO_COMP      if BCD_nan in (x, y)
    // GREATER_THAN if x > y
    // LESS_THAN    if y > x
    // EQUAL_TO     else
    if (unlikely(x.nan || y.nan))   {
        return NO_COMP;
    }
    if (x.negative != y.negative)   {
        return (x.negative) ? LESS_THAN : GREATER_THAN;
    }
    if (x.decimal_digits != y.decimal_digits)  {
        if (x.decimal_digits > y.decimal_digits)    {
            return (x.negative) ? LESS_THAN : GREATER_THAN;
        }
        return (x.negative) ? GREATER_THAN : LESS_THAN;
    }
    for (size_t i = x.bcd_digits - 1; i != -1; i--) {
        if (x.digits[i] != y.digits[i]) {
            if (x.negative) {
                return (x.digits[i] > y.digits[i]) ? LESS_THAN : GREATER_THAN;
            }
            return (x.digits[i] > y.digits[i]) ? GREATER_THAN : LESS_THAN;
        }
    }
    return EQUAL_TO;
}

BCD_int add_bcd(BCD_int x, BCD_int y)   {
    // performing this on two n-digit numbers will take O(n) time
    if (unlikely(x.nan || y.nan))   {
        BCD_int ret = BCD_nan;
        ret.error = ADD_NAN;
        return ret;
    }
    if (unlikely(x.zero && y.zero)) {
        return BCD_zero;
    }
    if (unlikely(x.zero))   {
        return copy_BCD_int(y);
    }
    if (unlikely(y.zero))   {
        return copy_BCD_int(x);
    }
    if (x.negative != y.negative)   {
        // if signs don't match, absolute value would go down.
        // that means we need to flip y's sign and move through sub_bcd()
        y.negative = !y.negative;
        return sub_bcd(x, y);
    }
    BCD_int z;
    size_t i, min_digits = min(x.bcd_digits, y.bcd_digits), max_digits = max(x.bcd_digits, y.bcd_digits);
    z.nan = false;
    z.constant = false;
    z.zero = false;  // result can't be zero because x and y are non-zero and share a sign
    z.negative = x.negative;  // we know this is also y.negative
    z.digits = (packed_BCD_pair *) malloc(sizeof(packed_BCD_pair) * (max_digits + 1));
    packed_BCD_pair a, b, c;
    bool overflow = false;
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
            #if (!defined(NO_ASSEMBLY) && X86_COMPILER)
                // if on these architectures, there's assembly tricks to adjust BCD in-silicon
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
            #else
                // otherwise fall back to doing it in C
                c = a + b;                             // set c to be the result of (a + b) % 0x100
                if ((overflow = (a > 0x99 - b)))    {  // if c would overflow the decimal range
                    c += 0x60;                         // and add 0x60 to make a decimal digit
                }
                if (((a & 0xF) + (b & 0xF)) > 9)    {  // if the lower nibble be bigger than 9
                    c += 0x06;                         // add 6 to make a decimal digit
                }
            #endif
            }
        z.digits[i] = c;
    }
    if (x.bcd_digits < y.bcd_digits)    {
        x = y;
    }
    for (; overflow && i < max_digits; i++) {  // while there's overflow and digits, continue adding
        a = x.digits[i] + overflow;
        if ((a & 0x0F) == 0x0A) {  // since all that's left is overflow, we don't need to check ranges
            a += 0x06;
        }
        if ((overflow = ((a & 0xF0) == 0xA0)))  {
            a += 0x60;
        }
        z.digits[i] = a;
    }
    for (; i < max_digits; i++) {  // if there's no more overflow, but still digits left, copy directly
        z.digits[i] = x.digits[i];
    }
    z.digits[max_digits] = overflow;
    z.bcd_digits = max_digits + overflow;
    if (overflow)   {
        z.decimal_digits = max_digits * 2 + 1;
    }
    else if (z.digits[max_digits - 1] & 0xF0)   {
        z.decimal_digits = max_digits * 2;
    }
    else    {
        z.decimal_digits = max_digits * 2 - 1;
    }
    z.even = !(z.digits[0] % 2);
    return z;
}

inline BCD_int inc_bcd(BCD_int x)   {
    return add_bcd(x, BCD_one);
}

BCD_int sub_bcd(BCD_int x, BCD_int y)   {
    if (unlikely(x.nan || y.nan))   {
        BCD_int ret = BCD_nan;
        ret.error = SUB_NAN;
        return ret;
    }
    if (unlikely(x.zero && y.zero)) {
        return BCD_zero;
    }
    if (unlikely(x.zero))   {
        BCD_int ret = copy_BCD_int(y);
        ret.negative = !ret.negative;
        return ret;
    }
    if (unlikely(y.zero))   {
        return copy_BCD_int(x);
    }
    if (x.negative != y.negative)   {
        // if signs don't match, absolute value would go up.
        // that means we need to flip y's sign and move through add_bcd()
        y.negative = !y.negative;
        return add_bcd(x, y);
    }
    signed char cmp = cmp_bcd(x, y);
    BCD_int z;
    if (unlikely((z.zero = !cmp)))  {  // x and y can't be NaN, so this is safe
        return BCD_zero;
    }
    z.negative = (cmp == -1);
    z.nan = false;
    z.constant = false;
    size_t i;
    const size_t min_digits = min(x.bcd_digits, y.bcd_digits), max_digits = max(x.bcd_digits, y.bcd_digits);
    z.digits = (packed_BCD_pair *) malloc(sizeof(packed_BCD_pair) * max_digits);
    packed_BCD_pair a, b, c;
    bool carry = false;
    for (i = 0; i < min_digits; i++) {
        a = x.digits[i];
        b = y.digits[i];
        b += carry;  // incorporate carry from last pair
        #if (!defined(NO_ASSEMBLY) && X86_COMPILER)
            // if on these architectures, there's assembly tricks to adjust BCD in-silicon
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
        #else
            // otherwise fall back to doing it in C
            c = a - b;                             // set c to be the result of (a - b) % 0x100
            if ((carry = (c & 0xF0) > 0x99))    {  // if c would overflow the decimal range
                c -= 0x60;                         // and subtract 0x60 to make a decimal digit
            }
            if ((c & 0x0F) > 9)  {                 // if the lower nibble be bigger than 9
                c -= 0x06;                         // subtract 6 to make a decimal digit
            }
        #endif
        z.digits[i] = c;
    }
    if (x.bcd_digits < y.bcd_digits)    {
        x = y;
    }
    for (; carry && i < max_digits; i++) {  // while there's carry and digits, continue adding
        a = x.digits[i] - carry;
        if ((a & 0x0F) == 0x0F) {  // since all that's left is carry, we don't need to check ranges
            a -= 0x06;
        }
        if ((carry = ((a & 0xF0) == 0xF0)))  {
            a -= 0x60;
        }
        z.digits[i] = a;
    }
    for (; i < max_digits; i++) {  // if there's no more carry, but still digits left, copy directly
        z.digits[i] = x.digits[i];
    }
    while (--i != -1)   {
        if (z.digits[i])    {
            z.bcd_digits = i + 1;
            if (z.digits[i] & 0xF0) {
                z.decimal_digits = 2 * z.bcd_digits;
            }
            else    {
                z.decimal_digits = 2 * z.bcd_digits - 1;
            }
            z.even = !(z.digits[0] % 2);
            return z;
        }
    }
    // should not get here, but just in case
    free_BCD_int(&z);
    return BCD_zero;
}


inline BCD_int dec_bcd(BCD_int x)   {
    return sub_bcd(x, BCD_one);
}

BCD_int mul_bcd(BCD_int x, BCD_int y)   {
    // multiplies two BCD ints by breaking them down into their component bytes and adding the results
    // this takes O(log_100(x) * log_100(y) * log_100(xy)) time
    BCD_int answer = BCD_zero, addend;
    if (unlikely(x.nan || y.nan))   {
        answer = BCD_nan;
        answer.error = MUL_NAN;
        return answer;
    }
    if (unlikely(x.zero || y.zero)) {
        return answer;
    }
    size_t i, j;
    uint16_t staging;
    uintmax_t ipow_10 = 0, pow_10;
    for (i = 0; i < x.bcd_digits; i++, ipow_10 += 2)  {
        for (j = 0, pow_10 = ipow_10; j < y.bcd_digits; j++, pow_10 += 2) {
            staging = mul_dig_pair(x.digits[i], y.digits[j]);
            if (staging == 0)   {
                continue;
            }
            if (likely(pow_10)) {
                addend = new_BCD_int2(staging, false);
                imul_bcd_pow_10(&addend, pow_10);  // this was not added to performance analysis
            }
            else    {
                addend = new_BCD_int2(staging, false);;
            }
            iadd_bcd(&answer, addend);
            free_BCD_int(&addend);
        }
    }
    answer.negative = !(x.negative == y.negative);
    return answer;
}

inline BCD_int div_bcd(BCD_int x, BCD_int y)    {
    BCD_int mod, div = divmod_bcd(x, y, &mod);
    free_BCD_int(&mod);
    return div;
}

inline BCD_int mod_bcd(BCD_int x, BCD_int y)    {
    BCD_int mod, div = divmod_bcd(x, y, &mod);
    free_BCD_int(&div);
    return mod;
}

BCD_int divmod_bcd(BCD_int x, BCD_int y, BCD_int *mod)  {
    // TODO: optimize this similar to mul_bcd_cuint() by stepping by 10s when possible
    BCD_int div = BCD_zero;
    if (unlikely(x.nan || y.nan || y.zero)) {
        BCD_int fail = BCD_nan;
        fail.error = (y.zero) ? DIV_ZERO : DIV_NAN;
        *mod = fail;
        return fail;
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
    div.negative = (x_negative != y_negative);
    if (div.negative)   {
        if (!tmp.zero)  {
            idec_bcd(&div);
        }
        *mod = sub_bcd(y, tmp);
        free_BCD_int(&tmp);
    }
    else    {
        *mod = tmp;
    }
    mod->negative = y_negative;
    return div;
}

BCD_int pow_bcd(BCD_int x, BCD_int y)   {
    // this takes O(y * 2log_100(x)^3) time
    if (unlikely(x.nan || y.nan || y.negative)) {
        BCD_int ret = BCD_nan;
        ret.error = (y.negative) ? POW_NEG : POW_NAN;
        return ret;
    }
    BCD_int answer = BCD_one;
    while (!y.zero) {
        imul_bcd(&answer, x);
        idec_bcd(&y);
    }
    return answer;
}

BCD_int factorial_bcd(BCD_int x)    {
    if (unlikely(x.nan || x.negative))  {
        BCD_int ret = BCD_nan;
        ret.error = (x.nan) ? FACT_NAN : FACT_NEG;
        return ret;
    }
    if (unlikely(x.zero || (x.decimal_digits == 1 && x.digits[0] == 1)))    {
        return BCD_one;
    }
    BCD_int i = dec_bcd(x), ret = copy_BCD_int(x);
    while (!i.zero) {
        imul_bcd(&ret, i);
        idec_bcd(&i);
    }
    free_BCD_int(&i);
    return ret;
}

inline void iadd_bcd(BCD_int *x, BCD_int y) {
    BCD_int ret = add_bcd(*x, y);
    free_BCD_int(x);
    *x = ret;
}

inline void iinc_bcd(BCD_int *x)    {
    BCD_int ret = inc_bcd(*x);
    free_BCD_int(x);
    *x = ret;
}

inline void isub_bcd(BCD_int *x, BCD_int y) {
    BCD_int ret = sub_bcd(*x, y);
    free_BCD_int(x);
    *x = ret;
}

inline void idec_bcd(BCD_int *x)    {
    BCD_int ret = dec_bcd(*x);
    free_BCD_int(x);
    *x = ret;
}

inline void imul_bcd(BCD_int *x, BCD_int y) {
    BCD_int ret = mul_bcd(*x, y);
    free_BCD_int(x);
    *x = ret;
}

inline void idiv_bcd(BCD_int *x, BCD_int y)    {
    BCD_int ret = div_bcd(*x, y);
    free_BCD_int(x);
    *x = ret;
}

inline void imod_bcd(BCD_int *x, BCD_int y)    {
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

inline void ipow_bcd(BCD_int *x, BCD_int y) {
    BCD_int ret = pow_bcd(*x, y);
    free_BCD_int(x);
    *x = ret;
}

inline void ifactorial_bcd(BCD_int *x)   {
    BCD_int ret = factorial_bcd(*x);
    free_BCD_int(x);
    *x = ret;
}

inline BCD_int mul_bcd_cint(BCD_int x, intmax_t y)  {
    if (y < 0)  {
        BCD_int ret = mul_bcd_cuint(x, (uintmax_t) (-y));
        ret.negative = !ret.negative;
        return ret;
    }
    return mul_bcd_cuint(x, (uintmax_t) y);
}

BCD_int mul_bcd_cuint(BCD_int x, uintmax_t y)   {
    // this takes roughly O(log(y) * log(x)) time, and is nearly linear if y is a multiple of 10
    // it works by breaking the multiplication down into groups of addition
    // full formula for performance is something like:
    // y = 10^a * b
    // f(i) = sum(n = 1 thru 19, (i % 10^(n + 1)) / 10^n) + i / 10^20
    // bool(i) = 1 if i else 0
    // time = O(bool(a) * log_100(x) + f(b) * log_100(xy))
    if (unlikely(x.nan))    {
        BCD_int ret = BCD_nan;
        ret.error = MUL_NAN;
        return ret;
    }
    if (unlikely(!y || x.zero)) {
        return BCD_zero;
    }
    unsigned char tens = 0;  // will up size when there's an 848-bit system
    BCD_int ret, mul_by_power_10;
    // first remove factors of ten
    while (y % 10 == 0) {
        y /= 10;
        ++tens;
    }
    if (tens)   {
        ret = mul_bcd_pow_10(x, tens);
    }
    else    {
        ret = BCD_zero;
    }
    // then for decreasing powers of ten, batch additions
    uintmax_t p = (sizeof(uintmax_t) == 16) ? MAX_POW_10_128 : MAX_POW_10_64;
    tens = (sizeof(uintmax_t) == 16) ? POW_OF_MAX_POW_10_128 : POW_OF_MAX_POW_10_64;
    for (; p > 1; p /= 10, --tens)  {
        while (y >= p)  {
            mul_by_power_10 = mul_bcd_pow_10(x, tens);
            iadd_bcd(&ret, mul_by_power_10);
            free_BCD_int(&mul_by_power_10);
            y -= p;
        }
    }
    // then do simple addition
    while (y--) {
        iadd_bcd(&ret, x);
    }
    return ret;
}

BCD_int mul_bcd_pow_10(BCD_int x, uintmax_t tens)   {
    // this takes O(log_100(x)) time. Note that it's significantly faster if tens is even
    // returns x * 10^tens
    BCD_int ret;
    if (unlikely(x.nan))    {
        ret = BCD_nan;
        ret.error = SHIFT_NAN;
        return ret;
    }
    if (unlikely(x.zero))   {
        return BCD_zero;
    }
    if (unlikely(!tens))    {
        return copy_BCD_int(x);
    }
    ret.zero = false;
    ret.nan = false;
    ret.constant = false;
    ret.even = x.even || !!tens;
    ret.negative = x.negative;
    ret.decimal_digits = x.decimal_digits + tens;
    ret.bcd_digits = (ret.decimal_digits + 1) / 2;
    ret.digits = (packed_BCD_pair *) calloc(ret.bcd_digits, sizeof(packed_BCD_pair));
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

inline BCD_int shift_bcd_left(BCD_int x, uintmax_t tens)    {
    return mul_bcd_pow_10(x, tens);
}

BCD_int div_bcd_pow_10(BCD_int a, uintmax_t tens)   {
    if (unlikely(a.nan))    {
        BCD_int ret = BCD_nan;
        ret.error = SHIFT_NAN;
        return ret;
    }
    if (unlikely(!tens))    {
        return copy_BCD_int(a);
    }
    if (tens >= a.decimal_digits)   {
        return BCD_zero;
    }
    BCD_int ret;
    ret.negative = a.negative;
    ret.zero = false;
    ret.nan = false;
    ret.constant = false;
    ret.decimal_digits = a.decimal_digits - tens;
    if (tens % 2 == 0)  {
        ret.bcd_digits = a.bcd_digits - tens / 2;
        ret.digits = (packed_BCD_pair *) malloc(sizeof(packed_BCD_pair) * ret.bcd_digits);
        memcpy(ret.digits, a.digits + tens / 2, ret.bcd_digits);
    }
    else    {
        // TODO
    }
    ret.even = !(ret.digits[0] % 2);
    return ret;
}

inline BCD_int shift_bcd_right(BCD_int a, uintmax_t tens)   {
    return div_bcd_pow_10(a, tens);
}

inline BCD_int pow_cuint_cuint(uintmax_t x, uintmax_t y)    {
    // this takes roughly O(xylog_100(xy)) time
    BCD_int answer = BCD_one;
    while (y--) {
        imul_bcd_cuint(&answer, x);
    }
    return answer;
}

inline BCD_int pow_cint_cuint(intmax_t x, uintmax_t y) {
    // this takes roughly O(xylog_100(xy)) time
    BCD_int answer = BCD_one;
    while (y--) {
        imul_bcd_cint(&answer, x);
    }
    return answer;
}

inline void imul_bcd_cuint(BCD_int *x, uintmax_t y) {
    BCD_int ret = mul_bcd_cuint(*x, y);
    free_BCD_int(x);
    *x = ret;
}

inline void imul_bcd_cint(BCD_int *x, intmax_t y)   {
    BCD_int ret = mul_bcd_cint(*x, y);
    free_BCD_int(x);
    *x = ret;
}

inline void imul_bcd_pow_10(BCD_int *x, uintmax_t tens) {
    BCD_int ret = mul_bcd_pow_10(*x, tens);
    free_BCD_int(x);
    *x = ret;
}

inline void ishift_bcd_left(BCD_int *x, uintmax_t tens)    {
    imul_bcd_pow_10(x, tens);
}

inline void idiv_bcd_pow_10(BCD_int *a, uintmax_t tens) {
    BCD_int ret = mul_bcd_cuint(*a, tens);
    free_BCD_int(a);
    *a = ret;
}

inline void ishift_bcd_right(BCD_int *a, uintmax_t tens)   {
    idiv_bcd_pow_10(a, tens);
}

inline uint16_t mul_dig_pair(packed_BCD_pair ab, packed_BCD_pair cd)    {
    // multiplies two digits pairs and returns an unsigned C short. valid range is 0 thru 9801
    unsigned char a, b, c, d;
    // first unpack the digits
    a = ab >> 4;
    b = ab & 0xF;
    c = cd >> 4;
    d = cd & 0xF;
    // then solve by FOIL
    return 100 * a * c + 10 * (a * d + b * c) + b * d;
}

void print_bcd(BCD_int x)   {
    if (unlikely(x.nan))    {
        printf("NaN");
        return;
    }
    if (unlikely(x.zero))   {
        printf("0");
        return;
    }
    if (x.negative) {
        printf("-");
    }
    size_t i = x.bcd_digits - 1;
    printf("%x", x.digits[i]);
    if (i--)    {
        for (; i != -1; i--)  {
            printf("%02x", x.digits[i]);
        }
    }
}

inline void print_bcd_ln(BCD_int x) {
    print_bcd(x);
    printf("\n");
}
