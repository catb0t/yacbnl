#ifndef YACBNL
#define YACBNL
/*
  YACBNL Yet Another C BigNum Library
  Copyright (C) 2017 Cat Stevens

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
  atom_t: an unsigned one-byte number "atom" element 0-FF
  bn: big number, usually a struct type or array repr (see bignum_t)
  bna: big number array; any array of atom_t
  b10 / b256: base 10, base 256
  zenz: base 256-related
  1b / 2b: one-byte and two-byte number interpretation
  u16: a 16 bit integer type
  u64: pertaining to a 64 bit integer type or non-float string representation
  ldbl: long double; a real / floating representation
  frac / flot: decimal sub-1 part of a number (right of decimal separator)
  int: integral part of a number (left of decimal separator in ltr)
  samb: single address, multi byte (see addr_interp.c)
  digits: refers usually to a base 10 string or array represenation of a number value
*/

#ifndef BN_COMMON_H
#define BN_COMMON_H

#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <inttypes.h>

// size of the headers for each kind of array
#define HEADER_OFFSET     ((atom_t) 4)
#define HEADER_OFFSET_BIG ((atom_t) 6)

#define DEC_BASE    10
#define ZENZ_BASE   256
#define COMPARE_EPS 1e-11 // (default) epsilon for fp comparisons
#define CHAR_DIGIT_DIFF ((uint8_t) 48) // difference between character #0 and the character '0'

// maximum significant figures for conversions to hardware types
#ifndef MAX_EXPORT_SIGFIGS
  #define MAX_EXPORT_SIGFIGS   ((UINT8_MAX * 2) - (HEADER_OFFSET * 2))
#endif

// and the same for big arrays
#ifndef MAX_EXPORT_SIGFIGS_BIG
  #define MAX_EXPORT_SIGFIGS_BIG ((UINT16_MAX * 2) - (HEADER_OFFSET_BIG * 2))
#endif

// number of kept significant figures for long doubles. probably obsolete
/*#ifndef MAX_DOUBLE_KEPT_FIGS
  #define MAX_DOUBLE_KEPT_FIGS 200
#endif
*/

#ifndef MAX_PRIMITIVE_LDBL_DIGITS
  #define MAX_PRIMITIVE_LDBL_DIGITS 50 // can be higher maybe
#endif

// volatile const uint64_t x = 12345678901234567890UL;
#ifndef MAX_U64_DIGITS
  #define MAX_U64_DIGITS 20
#endif

/* only a fallback for strn* functions in the rare case no null terminator is found */
#ifndef MAX_STR_LDBL_DIGITS
  #define MAX_STR_LDBL_DIGITS 10000
#endif

#ifndef DECIMAL_SEPARATOR_STR
  #define DECIMAL_SEPARATOR_STR "."
#endif

/*
  whether the compile process is going to prefer string-based operations or math-based operations
  for e.g determining whether to use strings or logarithms for getting the number of digits in an integer
*/
#ifdef PREFER_CHAR_CONV
  #pragma message("preferring slower O(n) char operations over O(1) floating point math")
#endif

/* TYPEDEFS */

typedef long double ldbl_t;
typedef uint8_t     atom_t;

typedef struct st_bignum_t {

  /*

  TYPE, INT LEN, FRAC LEN, FLAGS, DATA...
  TYPE, INT LEN 1, INT LEN 2, FRAC LEN 1, FRAC LEN 2, FLAGS, DATA... (big)


    array + 0 = the type of this array

    ARRAY TYPES:
      if the type byte & TYP_BIG is true, the array is big. this means it uses a
        two-byte addressing scheme. in the following array:
        { TYP_BIG, 0, 2, 0, 1, FL_NONE, 1, 2, 1 }
        the BIG flag is set, and four total bytes (2 for the integral, 2 for the
        fractional, in little endian order) are reserved for describing the length of
        the array. these value pairs can be interpreted by the functions
        u16_to_twoba (uint16_t to two byte address) and twoba_to_u16 (two byte
        address to uint16_t).
        if the type byte does not share bits with TYP_BIG, then a standard
        single-byte addressing mode is in use.
        !! the type of the first array in a struct does not determine the type of
        the second extension array; each has its own metadata.

      if the type byte & TYP_ZENZ is true, then the array's data elements after
        the metadata are in base 256. that is, rather than store each digit of the
        number as a base 10 digit from 0-9, this array represents an equivalent value
        using all 8 bits of each array element.
        it would take 3 full base 10 bytes (each only using the first two bits) to
        represent the value 255, but only one base 256 byte, using all 8 bits.
  */

  atom_t
    * value, // the main value
    * imgry, // imaginary part
    * fracl, // fractional part (numerator / denom)
    * vextn; // reference this if (...) the first is determined to possibly by filled up
  struct st_bignum_t* expt;

} bignum_t;

// highest value for these bases. self-explanatory but erase magic numbers
#define B256_HIGH 0x100
#define B10_HIGH  0xA

/* composable flags */
/* array types */
#define TYP_NONE  0x0  /* absolutely normal in every way. no need to apply this, obviously, but you can test against it */
#define TYP_BIG   0x01 /* this array is big (2 byte addressing mode) */
#define TYP_ZENZ  0x02 /* array uses base 256 rather than base 10 (from the word zenzizenzizenzic) */
#define TYP_OVERF 0x04 /* this array's intended value would overflow this array; combine its data with another */
#define TYP_EXTN  0x08 /* this array is the extension of another array whose value is overflowed (see TYP_OVERF) */

/*
  these apply to number values themselves, and can be composed, such that
  negative NaN and negative INF are possible as they should be
*/
#define FL_NONE 0x0  /* none of the following */
#define FL_SIGN 0x01 /* this number is negative */
#define FL_NAN  0x02 /* this number is not a number */
#define FL_INF  0x04 /* this number is infinity */

/*
  handy macros which may expand indefinitely
  for getting data about arrays more easily
*/
/*
  the following are about the METADATA byte -- apply them to the VALUE of the 0th
  index to learn something about that TYP slot
*/
// whether this metadata indicates base 256
#define    meta_is_base256(metadata) (metadata & TYP_ZENZ)
// whether this metadata indicates the big, two byte addressing mode for the array
#define        meta_is_big(metadata) (metadata & TYP_BIG)
// the size of the header offset for the encompassing array
#define meta_header_offset(metadata) (meta_is_big(metadata) ? HEADER_OFFSET_BIG : HEADER_OFFSET)

/*
  the following are about big num arrays themselves, not any particular byte
*/
// whether this array is base 256
#define    bna_is_base256(bna) (meta_is_base256(bna[0]))
// whether it is big
#define        bna_is_big(bna) (meta_is_big(bna[0]))
// the size of the header offset
#define bna_header_offset(bna) (meta_header_offset(bna[0]))
// the "real length" of the constituent parts of this array
#define      bna_real_len(bna) (bna_int_len(bna) + bna_frac_len(bna) + bna_header_offset(bna))
// the flags set for this array
#define         bna_flags(bna) ((bna)[bna_header_offset((bna)) - 1])
// the length of the integer part
#define       bna_int_len(bna) ( bna_is_big(bna) ? samb_twoarray_to_u16((bna) + 1) : (bna)[1] )
// the length of the fractional part
#define      bna_frac_len(bna) ( bna_is_big(bna) ? samb_twoarray_to_u16((bna) + 3) : (bna)[2] )

#define  bna_new_1b_10_u64(value, flags) to_digit_array(0, value, flags, TYP_NONE)
#define bna_new_1b_256_u64(value, flags) to_digit_array(0, value, flags, TYP_ZENZ)

#define  bna_new_2b_10_u64(value, flags) to_digit_array(0, value, flags, TYP_BIG)
#define bna_new_2b_256_u64(value, flags) to_digit_array(0, value, flags, TYP_BIG | TYP_ZENZ)

/* */

#define  bna_new_1b_10_ldbl(value) to_digit_array(value, 0, flags, TYP_NONE)
#define bna_new_1b_256_ldbl(value) to_digit_array(value, 0, flags, TYP_ZENZ)

#define  bna_new_2b_10_ldbl(value) to_digit_array(value, 0, flags, TYP_BIG)
#define bna_new_2b_256_ldbl(value) to_digit_array(value, 0, flags, TYP_BIG | TYP_ZENZ)



// multiplies a size by the size of the typename to get the size of a space
#define        sz(type, n) ( ((size_t) n) * (sizeof (type)) )
// allocates, but does not clean -- a shorthand for writing malloc(n * sizeof(type))
#define  alloc(type, size) malloc(( (size_t) size) * sizeof (type))
// same, but cleans (zeroes) the bytes with calloc
#define zalloc(type, size) calloc(( (size_t) size),  sizeof (type))
#define     macrogetval(x) #x
#define       stringify(x) macrogetval(x)

#ifndef set_out_param
  #define set_out_param(name, value) do { if ( NULL != (name) ){ *name = value; } } while(0)
#endif

#ifndef string_is_sempty
  #define string_is_sempty(str, n) (NULL == str || ! strnlen_c(str, n))
#endif

/* individual values */
typedef enum {
  BN_NONE = 0,

  BN_SIGN = FL_SIGN,
  BN_SNAN = FL_SIGN | FL_NAN,
  BN_SINF = FL_SIGN | FL_INF,

  BN_NAN  = FL_NAN,
  BN_INF  = FL_INF

} bignum_flag_t;

/* misc_util */
float                log256f (const float x);
bool             compare_eps (const ldbl_t a, const ldbl_t b, const ldbl_t eps);
atom_t      count_digits_u64 (const uint64_t x);
atom_t  indexable_digits_u64 (const uint64_t x);
atom_t count_b256_digits_u64 (const uint64_t x);
uint16_t count_b256_digits_b10_digits (const char* const digits);
atom_t    get_left_nth_digit (const uint64_t x, const atom_t n);
atom_t     count_frac_digits (const char* const str);
atom_t   find_frac_beginning (const char* const str);
bool             raw_is_zero (const atom_t* const digits, const uint16_t len);

size_t        strnlen_c (const char* const s, const size_t maxsize);
char*         strndup_c (const char* const s, size_t const n);
char*       str_reverse (const char* const str);
size_t        str_count (const char* const str, const char find);
char* make_empty_string (void);
void say_atom_t_ptr (const atom_t* const data, const uint16_t len);

uint16_t array_span (const atom_t* arr, const uint16_t arr_len, const bool accept, const atom_t* const vals, const uint16_t vals_len);

bool   array_contains (const atom_t* const arr, const uint16_t len, const atom_t value);
atom_t*  array_concat (const atom_t* const a, const atom_t* const b, const uint16_t a_len, const uint16_t b_len);
atom_t* array_reverse (const atom_t* const arr, const uint16_t len);

atom_t*         array_trim_leading_zeroes (const atom_t* const bn);
atom_t*        array_trim_trailing_zeroes (const atom_t* const bn);
atom_t* array_trim_trailing_zeroes_simple (const atom_t* const bn, const uint16_t len);
atom_t*  array_trim_leading_zeroes_simple (const atom_t* const bn, const uint16_t len);


/* bignum_t */
bignum_t* bignum_ctor (const ldbl_t ldbl, const uint64_t u64, const atom_t flags, const bignum_t * const * const opt_vals);
bignum_t* bignum_copy (const bignum_t* const bn, const bool no_recurse_optionals);

/* array creation */
atom_t* make_array_header (const atom_t metadata, const uint16_t int_digits, const uint16_t flot_digits, const atom_t flags);
atom_t* to_digit_array (const ldbl_t ldbl_in, const uint64_t u64, const atom_t value_flags, const atom_t metadata);

/* 2 byte addressing stuff */
void        samb_u16_to_twoba (const uint16_t n, atom_t* const ah, atom_t* const al);
uint16_t    samb_twoba_to_u16 (const atom_t ah, const atom_t al);
uint16_t samb_twoarray_to_u16 (const atom_t arr[static 2]);

/* these are raw atom_t arrays, not bignum structures */

/* base 10 conversions */
char*   b10_to_ldbl_digits (const atom_t* const digits, const uint16_t len, const uint16_t int_len);
char*    b10_to_u64_digits (const atom_t* const digits, const uint16_t len);
uint64_t        b10_to_u64 (const atom_t* const digits, const uint16_t len);
atom_t* ldbl_digits_to_b10 (const char* const ldbl_digits, uint16_t* const len, uint16_t* const int_len, const bool little_endian);
atom_t*         u64_to_b10 (const uint64_t value, uint16_t* const len, const bool little_endian);
atom_t*  u64_digits_to_b10 (const char* const digits, uint16_t* const len, const bool little_endian);

uint16_t b10_to_u16 (const atom_t* const, const uint16_t len);

/* base 256 conversions */
char*     b256_to_ldbl_digits (const atom_t* const digits, const uint16_t len, const uint16_t int_len);
char*     b256_to_u64_digits (const atom_t* const digits, const uint16_t len);
uint64_t         b256_to_u64 (const atom_t* const digits, const uint16_t len);
atom_t*  ldbl_digits_to_b256 (const char* const ldbl_digits, uint16_t* const len, uint16_t* const int_len, const bool little_endian);
atom_t*          u64_to_b256 (const uint64_t value, uint16_t* const len, const bool little_endian);
atom_t*   u64_digits_to_b256 (const char* const digits, uint16_t* const len, const bool little_endian);

/*
  raw math primitives required to implement basic functionality
  NOT user-friendly math functions

  math_primitive_base10
  all these functions are real unsigned!!
*/
atom_t* succ_b10 (const atom_t* const n, const uint16_t len, const uint16_t int_len, const uint16_t precision, uint16_t* const out_len, uint16_t* const out_int_len);
atom_t* pred_b10 (const atom_t* const n, const uint16_t len, const uint16_t int_len, const uint16_t precision, uint16_t* const out_len, uint16_t* const out_int_len);
// natural log base e (2.718...)
atom_t* log_b10(const atom_t* const n, const uint16_t n_len, const uint16_t n_int_len, uint16_t* const out_len, uint16_t* const out_int_len);
// log base n of x
atom_t* logn_b10 (const atom_t* const base /* n */, const uint16_t base_len, const uint16_t base_int_len, const atom_t* const n /* x */, const uint16_t n_len, const uint16_t n_int_len, /* out */ uint16_t* const out_len, /* out */ uint16_t* const out_int_len);
atom_t* add_b10 (const atom_t* const a, const atom_t* const b);
atom_t* sub_b10 (const atom_t* const a, const atom_t* const b);
atom_t* mul_b10 (const atom_t* const a, const atom_t* const b);
atom_t* div_b10 (const atom_t* const a, const uint16_t a_len, const uint16_t a_int_len, const atom_t* const b, const uint16_t b_len, const uint16_t b_int_len, uint16_t* const out_len, uint16_t* const out_int_len);

// x^n
atom_t* pow_b10 (const atom_t* const x, const atom_t* const n);

atom_t* floor_b10 (const atom_t* const n, const uint16_t len, const uint16_t int_len);

#endif /* end of include guard: BN_COMMON_H */

#ifndef MATH_PRIMITIVE_BASE10
#define MATH_PRIMITIVE_BASE10
/* simple unsigned real number math */
static atom_t* impl_pred_b10_int (const atom_t* const n, const uint16_t len, uint16_t* const out_len) {
  // last digit is not 0
  if (0 != n[len - 1]) {
    set_out_param(out_len, len);
    // only the last digit changes, so copy all the preceding digits
    atom_t* const result = memcpy(alloc(atom_t, len), n, (uint16_t) (len - 1));
    // then take 1
    result[len - 1] = (atom_t) ((atom_t) n[len - 1] - 1);
    return result;
  } else {
    /*
      12300 -> 123 -> 122 -> 12299
      12100 -> 121 -> 120 -> 12099
      50000 -> 5 -> 4 -> 49999
      10000 -> 1 -> 0 -> 09999 -> 9999
    */
    // one or more last digits are 0
    atom_t* const z = zalloc(atom_t, 1);
    const uint16_t count_leading_nonzeroes = array_span(n, len, false, z, 1);
    free(z);
    const uint16_t count_trailing_zeroes = (uint16_t) (len - count_leading_nonzeroes);
    atom_t* const leading_digits = memcpy(alloc(atom_t, count_leading_nonzeroes), n, count_leading_nonzeroes);
    // take 1 from the first nonzero digit
    leading_digits[count_leading_nonzeroes - 1] = (atom_t) (leading_digits[count_leading_nonzeroes - 1] - 1);
    // space for 00321
    atom_t* const emplace_nines = alloc(atom_t, len);
    // 00321 -> 99221
    memcpy(emplace_nines, leading_digits, count_leading_nonzeroes);
    free(leading_digits);
    // emplace the needed number of nines
    memset(emplace_nines + count_leading_nonzeroes, 9, count_trailing_zeroes);
    // reverse: 99221 -> 12299
    // drop any leftover leading zeroes: 0999 -> 999
    atom_t* const final = array_trim_leading_zeroes_simple(emplace_nines, len);
    free(emplace_nines);
    // record if it was shortened by a digit
    const bool shortened = (1 == n[0]) && (1 == count_leading_nonzeroes);
    set_out_param(out_len, (uint16_t) (len - shortened));
    return final;
  }
}
static atom_t* impl_pred_b10_flot (const atom_t* const n, const uint16_t len, const uint16_t int_len, const uint16_t precision, uint16_t* const out_len, uint16_t* const out_int_len) {
  // going to traverse one more than int_len
  // preicision may be 0 to subtract 1 from a real
  const uint16_t focus_digit = (uint16_t) ((int_len + precision) - 1);
  if ( 0 != n[focus_digit] ) {
    // simple case: digit of interest is not 0 so just take 1
    set_out_param(out_len, len);
    set_out_param(out_int_len, int_len);
    atom_t* const result = memcpy(alloc(atom_t, len), n, len);
    // then take 1
    result[focus_digit] = (atom_t) ((atom_t) n[focus_digit] - 1);
    return result;
  } else {
    // the digit in focus is 0
    // we need to figure out what other digits are 0
    // taking 1 from a digit only affects the digits more significant than it
    puts("UNIMPLEMENTED");
    return NULL;
  }
}
/*
  atom_t*, uint16_t -> atom_t*, uint16_t
  the number before n
    before 0 is 0
    before 1 is 0
    before 9 is 8
    before 10 is 9
    before 11 is 10
    before 20 is 19
    before 100 is 99
    before 1.23 pr=2 is 1.22
    before 1.23 pr=1 is 1.13
    before 1.20 pr=0 is 0.23
    before 1.01 pr=1 is 0.91
    before 0.99 pr=2 is 0.98
*/
atom_t* pred_b10 (const atom_t* const n, const uint16_t len, const uint16_t int_len, const uint16_t precision, uint16_t* const out_len, uint16_t* const out_int_len) {
  if ( (len == int_len && raw_is_zero(n, int_len) ) || raw_is_zero(n, len) ) {
    set_out_param(out_len, 0);
    return zalloc(atom_t, 1);
  }
  if (len == int_len && 0 == precision) {
    return impl_pred_b10_int(n, len, out_len);
  } else {
    return impl_pred_b10_flot(n, len, int_len, precision, out_len, out_int_len);
  }
}
/*
  atom_t*, uint16_t -> atom_t*, uint16_t
*/
atom_t* succ_b10 (const atom_t* const n, const uint16_t len, const uint16_t int_len, const uint16_t precision, uint16_t* const out_len, uint16_t* const out_int_len) {
  if ( raw_is_zero(n, len) ) {
    set_out_param(out_len, 0);
    return zalloc(atom_t, 1);
  }
  (void) precision;
  (void) int_len;
  (void) out_int_len;
  return NULL;
}
atom_t* div_b10 (const atom_t* const a, const uint16_t a_len, const uint16_t a_int_len, const atom_t* const b, const uint16_t b_len, const uint16_t b_int_len, uint16_t* const out_len, uint16_t* const out_int_len) {
  (void) a;
  (void) a_len;
  (void) a_int_len;
  (void) b;
  (void) b_len;
  (void) b_int_len;
  (void) out_len;
  (void) out_int_len;
  return NULL;
}
atom_t* floor_b10 (const atom_t* const n, const uint16_t len, const uint16_t int_len) {
  (void) n;
  (void) len;
  (void) int_len;
  return NULL;
}
/*static const atom_t euler_constant[] = { 2, 7, 1, 8, 2, 8, 1, 8, 2, 8, 4, 5, 9, 0, 4, 5, 2, 3, 5, 3, 6, 0, 2, 8, 7, 4, 7, 1, 3, 5, 2, 6, 6, 2, 4, 9, 7, 7, 5, 7, 2, 4, 7, 0, 9, 3, 6, 9, 9, 9, 5, 9, 5, 7, 4, 9, 6, 6
};
static const uint16_t euler_constant_int_len = 1;
static const uint16_t euler_constant_len = 58;
*/
// i would like 10 digits of precision. is that too much to ask?
atom_t* log_b10(const atom_t* const n, const uint16_t n_len, const uint16_t n_int_len, uint16_t* const out_len, uint16_t* const out_int_len) {
  /* who knows??? */
  puts("UNIMPLEMENTED");
  (void) n;
  (void) n_int_len;
  (void) n_len;
  (void) out_int_len;
  (void) out_len;
  return NULL;
}
atom_t* logn_b10 (const atom_t* const base /* n */, const uint16_t base_len, const uint16_t base_int_len, const atom_t* const n /* x */, const uint16_t n_len, const uint16_t n_int_len, /* out */ uint16_t* const out_len, /* out */ uint16_t* const out_int_len) {
  uint16_t log_base_int_len = 0, log_base_len = 0;
  atom_t* const log_base = log_b10(base, base_len, base_int_len, &log_base_len, &log_base_int_len);
  uint16_t log_n_int_len = 0, log_n_len = 0;
  atom_t* const log_n = log_b10(n, n_len, n_int_len, &log_n_len, &log_n_int_len);
  atom_t* const result = div_b10(log_n, log_n_len, log_n_int_len, log_base, log_base_len, log_base_int_len, out_len, out_int_len);
  free(log_base);
  free(log_n);
  return result;
}
#endif /* end of include guard: MATH_PRIMITIVE_BASE10 */


#ifndef BASE10_H
#define BASE10_H
/*
  atom_t*, uint16_t, uint16_t -> char*
  convert a base 10 array into a string base 10 floating point number
  very simple operation from { 1 2 3 4 5 } with int_len = 3 -> "123.45"
  an empty string is returned when
    len is 0
    int_len is greater than len
    digits is NULL
*/
char* b10_to_ldbl_digits (const atom_t* const digits, const uint16_t len, const uint16_t int_len) {
  if (NULL == digits || ! len || int_len > len) { return make_empty_string(); }
  char* const int_part = alloc(char, int_len + 1);
  for (size_t i = 0; i < int_len; i++) {
    int_part[i] = (char) ((char) digits[i] + CHAR_DIGIT_DIFF);
  }
  int_part[int_len] = '\0';
  const size_t flot_len = (size_t) ((size_t) len - int_len);
  char* const flot_part = alloc(char, flot_len + 1);
  for (size_t i = 0; i < flot_len; i++) {
    flot_part[i] = (char) ((char) digits[int_len + i] + CHAR_DIGIT_DIFF);
  }
  flot_part[flot_len] = '\0';
  const bool dot = len != int_len;
  char* const final_concat = alloc(char, int_len + dot + flot_len);
  snprintf(final_concat, MAX_STR_LDBL_DIGITS, "%s%s%s", int_part, (dot ? DECIMAL_SEPARATOR_STR : ""), flot_part);
  free(int_part), free(flot_part);
  return final_concat;
}
/*
  atom_t*, uint16_t -> char*
  base 10 array into a string base 10 number
  very simple operation to subtract CHAR_DIGIT_DIFF
*/
char* b10_to_u64_digits (const atom_t* const digits, const uint16_t len) {
  char* const u64_str = alloc(char, len + 1);
  for (uint16_t i = 0; i < len; i++) {
    u64_str[i] = (char) (digits[i] + CHAR_DIGIT_DIFF);
  }
  u64_str[len] = '\0';
  return u64_str;
}
/*
  atom_t*, uint16_t -> uint64_t
  like b10_to_u64_digits but the conversion is to a number value
  the conversion may overflow, to avoid this check errno
    and use b10_to_u64_digits
*/
uint64_t b10_to_u64 (const atom_t* const digits, const uint16_t len) {
  char* const u64_str = b10_to_u64_digits(digits, len);
  const uint64_t final = strtoull(u64_str, NULL, DEC_BASE);
  free(u64_str);
  return final;
}
uint16_t b10_to_u16(const atom_t* const n, const uint16_t len) {
  (void) n; (void) len;
  return 0;
}
// string 123.45 to { 1 2 3 4 5 ... }
/*
  char*, bool -> atom_t*, uint16_t, uint16_t
  convert a string of base 10 long double (floating) digits to
*/
atom_t* ldbl_digits_to_b10 (const char* const ldbl_digits, /* out */ uint16_t* const len, /* out */ uint16_t* const int_len, const bool little_endian) {
  if ( (0 == strnlen_c(ldbl_digits, MAX_U64_DIGITS + 2)) || NULL == ldbl_digits || NULL == len || NULL == int_len) {
    set_out_param(len, 1);
    set_out_param(int_len, 1);
    return zalloc(atom_t, 1);
  }
  /* length of integer part before the decimal point */
  const uint16_t pre_dec = (uint16_t) strcspn(ldbl_digits, DECIMAL_SEPARATOR_STR),
              digits_len = (uint16_t) strnlen_c(ldbl_digits, MAX_STR_LDBL_DIGITS);
  char *const int_part  = strndup_c(ldbl_digits, pre_dec), // 1
       *const flot_part = strndup_c(ldbl_digits + pre_dec + 1, MAX_STR_LDBL_DIGITS);
  uint16_t i = 0;
  for (; int_part[i]; i++) {
    int_part[i]  = (char) ((char) int_part[i] - CHAR_DIGIT_DIFF);
  }
  const uint16_t int_part_len = i;
  for (i = 0; flot_part[i]; i++) {
    flot_part[i]  = (char) ((char) flot_part[i] - CHAR_DIGIT_DIFF);
  }
  const uint16_t flot_part_len = i;
  set_out_param(len, (uint16_t) ((uint16_t) int_part_len + flot_part_len));
  set_out_param(int_len, int_part_len);
  atom_t* const res = alloc(atom_t, digits_len);
  memcpy(res, int_part, int_part_len);
  memcpy(res + int_part_len + 1, flot_part, flot_part_len);
  free(int_part), free(flot_part);
  if (little_endian) {
    atom_t* const res_rev = array_reverse(res, (uint16_t)((uint16_t) int_part_len + flot_part_len));
    free(res);
    return res_rev;
  }
  return res;
}
atom_t* u64_to_b10 (const uint64_t value, /* out */ uint16_t* const len, const bool little_endian) {
  char* const str_digits = alloc(char, MAX_U64_DIGITS + 1);
  snprintf(str_digits, MAX_U64_DIGITS, "%" PRIu64, value);
  atom_t* const bytes = u64_digits_to_b10(str_digits, len, little_endian);
  free(str_digits);
  return bytes;
}
// string like 23948734 to atom_t array { 2 3 9 ... }
atom_t* u64_digits_to_b10 (const char* const digits, /* out */ uint16_t* const len, const bool little_endian) {
  const size_t digits_len = strnlen_c(digits, MAX_STR_LDBL_DIGITS);
  if (NULL == digits || 0 == digits_len || NULL == len) {
    return NULL;
  }
  set_out_param(len, (uint16_t) digits_len);
  atom_t* const as_b10 = alloc(atom_t, digits_len);
  for (uint16_t i = 0; i < digits_len; i++) {
    as_b10[i] = (atom_t) ((char) digits[i] - CHAR_DIGIT_DIFF);
  }
  if (little_endian) {
    atom_t* const rev_digits = array_reverse(as_b10, (uint16_t) digits_len);
    free(as_b10);
    return rev_digits;
  }
  return as_b10;
}
#endif


#ifndef BIGNUM_H
#define BIGNUM_H
/*
  ldbl_t, uint64_t, atom_t, bignum_t** -> bignum_t*
  create a new bignum out of primitives and values or give zero
*/
bignum_t* bignum_ctor (
  const ldbl_t   ldbl,
  const uint64_t u64,
  const atom_t flags,
  const bignum_t * const * const opt_vals
) {
  bool cx, fr, ex;
  if (NULL != opt_vals) {
    cx = NULL != opt_vals[0];
    fr = NULL != opt_vals[1];
    ex = NULL != opt_vals[2];
  } else {
    cx = fr = ex = 0;
  }
  /* create a stack-allocated version of the structure */
  bignum_t st_bn = {
    /* real value */
    .value = to_digit_array(ldbl, u64, flags, 0),
    /* imaginary part is determined */
    .imgry = cx
      ? memcpy(
          alloc(atom_t, bna_real_len(opt_vals[0]->value)),
          opt_vals[0]->value,
          sz(atom_t, bna_real_len(opt_vals[0]->value))
        )
      : zalloc(atom_t, HEADER_OFFSET),
    /* fractional (numerator / denominator) */
    .fracl = fr
      ? memcpy(
        alloc(atom_t, bna_real_len(opt_vals[1]->value)),
        opt_vals[1]->value,
        sz(atom_t, bna_real_len(opt_vals[1]->value))
      )
      : zalloc(atom_t, HEADER_OFFSET),
    /* exponent */
    .expt = ex
      ? bignum_copy(opt_vals[2], true)
      : bignum_ctor(0, 0, 0, NULL)
  };
  /* copy it to the heap */
  bignum_t* hp_bn = memcpy( alloc(bignum_t, 1), &st_bn, sizeof(bignum_t) );
  return hp_bn;
}
/*
  bignum_t* -> bignum_t*
  deep copy a bignum_t's properties but not its identity
*/
bignum_t* bignum_copy (const bignum_t* const bn, const bool no_recurse_optionals) {
  if (NULL == bn) {
    return zalloc(bignum_t, 1);
  }
  (void) no_recurse_optionals;
  return NULL;
}
#endif /* end of include guard: BIGNUM_H */


#ifndef BNA_H
#define BNA_H
static atom_t* impl_to_digit_array_ldbl (const ldbl_t ldbl,  const atom_t metadata, const atom_t flags);
static atom_t*  impl_to_digit_array_u64 (const uint64_t u64, const atom_t metadata, const atom_t flags);
/* create the first 4 or 6 bytes at the beginning of every digit array */
atom_t* make_array_header (const atom_t metadata, const uint16_t int_digits, const uint16_t flot_digits, const atom_t flags) {
  const atom_t  hdrlen = meta_header_offset(metadata);
  atom_t* const header = zalloc(atom_t, hdrlen);
  /* first byte is the type and base */
  header[0]          = metadata;
  /* last byte is flags */
  header[hdrlen - 1] = flags;
  if ( meta_is_big(metadata) ) {
    atom_t lens[] = { 0, 0, 0, 0 };
    /* set the lengths by using the addresses of the array elements */
    samb_u16_to_twoba(int_digits,  lens + 0, lens + 1);
    samb_u16_to_twoba(flot_digits, lens + 2, lens + 3);
    /* paste four bytes between the metadata and flags */
    memcpy(header + 1, &lens, sz(atom_t, 4) );
  } else {
    header[1] = (atom_t) int_digits;
    header[2] = (atom_t) flot_digits;
  }
  return header;
}
/*
  ldbl_t, uint64_t, atom_t -> atom_t*
  make a new bignum array "underlying" object from the first nonzero of two
  primitive arguments, or zero if both are.
  value metadata, like signedness, infiniteness and nan-ness should be indicated in
  flags, not some cryptic value for argument #1 ldbl_t
  for example, signed zero would best be represented by to_bn_array(0, 0, FL_SIGN, 0).
*/
atom_t* to_digit_array (const ldbl_t ldbl_in, const uint64_t u64, const atom_t value_flags, const atom_t metadata) {
  const atom_t flags = ldbl_in < 0 ? value_flags | FL_SIGN : value_flags;
  const ldbl_t ldbl  = ldbl_in < 0 ? fabsl(ldbl_in) : ldbl_in;
  // not zero so we'll use it
  if ( ! compare_eps(ldbl, 0.f, COMPARE_EPS) ) {
    return impl_to_digit_array_ldbl(ldbl, metadata, flags);
  // use this instead
  } else if ( 0 != u64 ) {
    return impl_to_digit_array_u64(u64, metadata, flags);
  } else /* both are zero, just give zero */ {
    return make_array_header(metadata, 0, 0, flags);
  }
  return NULL;
}
/* IMPLEMENTATIONS */
/*
  ldbl_t, atom_t, atom_t -> atom_t*
  represent a long double number as an array of digits
  the values of metadata and flags are copied into the resulting array
  the caller should have checked whether ldbl is within 1e-11 of 0, therefore,
    this check is not done
*/
static atom_t* impl_to_digit_array_ldbl (const ldbl_t ldbl, const atom_t metadata, const atom_t flags) {
  /* important metadata flags */
  const bool is_base256 = meta_is_base256(metadata),
                 is_big = meta_is_big(metadata);
  /* length of the entire header section */
  const atom_t hdrlen    = meta_header_offset(metadata);
  /* upper bound on significant figures we can store in one array */
  const uint32_t sigfigs = is_big ? MAX_EXPORT_SIGFIGS_BIG : MAX_EXPORT_SIGFIGS;
  /* put the entire value into a string, which may have trailing zeroes */
  char * const fullstr  = alloc(char, sigfigs + 3 /* separator + null */); // 1
  snprintf(fullstr, sigfigs + 2 /* sep */, "%LG", ldbl);
  const atom_t
    // already in fullstr form but the question is, which has lower time complexity
#ifdef PREFER_CHAR_CONV
    // naive string ops
    nint_digits  = (atom_t) strcspn(fullstr, DECIMAL_SEPARATOR_STR),
#else /* ! PREFER_CHAR_CONV */
    // O(1) maths
    nint_digits  = count_digits_u64( (uint64_t) floorl(ldbl) ),
#endif /* PREFER_CHAR_CONV */
    nflot_digits = count_frac_digits(fullstr);
  //printf("digits: %d %d\n", nint_digits, nflot_digits);
  //printf("!!! DBGPRN %LG %s\n", ldbl, fullstr);
  /* make space for the entire new data and store the metadata as a header */
  atom_t *  bn_tlated = alloc(atom_t, nint_digits + nflot_digits + hdrlen),  // 2
         * const init = make_array_header(metadata, nint_digits, nflot_digits, flags); // 3
  /* put the new header in the initial section of new data */
  memcpy(bn_tlated, init, sz(atom_t, hdrlen));
  free(init); // ~3
  if (is_base256) {
    /* convert fullstr to array representation */
    uint16_t len, int_len;
    /* little endian*/
    atom_t* as_digits = ldbl_digits_to_b256(fullstr, &len, &int_len, true);
    free(fullstr);
    /* just copy the data into the rest of the array */
    memcpy(bn_tlated + hdrlen, as_digits, len);
  } else /* base 10 */ {
      /* going to do the integral component */
#ifdef PREFER_CHAR_CONV
    /* more naive simple string operations */
    char* const integ_str = strndup_c(fullstr, nint_digits); // 4
    for (atom_t i = 0; i < nint_digits; i++) {
      bn_tlated[i + hdrlen] = (atom_t) ((unsigned) integ_str[i] - '0');
    }
    free(integ_str); // ~4
#else /* ! PREFER_CHAR_CONV */
    /* because it is integral we can do this part with integer math */
    for (atom_t i = 0; i < nint_digits; i++) {
      bn_tlated[i + hdrlen] = get_left_nth_digit( (uint64_t) floorl(ldbl), i);
    }
#endif /* PREFER_CHAR_CONV */
    /*
      going to do the fractional component
      could be done by multiplying the fractional part into integrality
      -- but that would not be worth the complexity, because it would require
      you to know the answer (the number of significant digits) ahead of time
    */
    const char* const frac_str = fullstr + find_frac_beginning(fullstr);
    for (size_t i = 0; i < nflot_digits; i++) {
      bn_tlated[i + hdrlen + nint_digits] = (atom_t) ((unsigned) frac_str[i] - '0');
    }
  }
  free(fullstr); //  ~1
  return bn_tlated; // 2
} /* impl_to_digit_array_ldbl */
/*
  uint64_t, atom_t, atom_t -> atom_t*
  represent a uint64_t value as an array of digits
  the values of metadata and flags are copied into the resulting array
  the caller should have checked whether ldbl is 0, therefore, this check is
    not done
*/
static atom_t* impl_to_digit_array_u64 (const uint64_t u64, const atom_t metadata, const atom_t flags) {
  /*
    TODO: add BIG support (?)
  */
  const bool using_base256 = meta_is_base256(metadata);
                    //is_big = meta_is_big(metadata);
  /* number of digits we'll need and the length of the header we'll need */
  const atom_t ndigits = using_base256 ?
                            count_b256_digits_u64(u64) :
                            count_digits_u64(u64),
                hdrlen = meta_header_offset(metadata);
  /* now we can allocate the right size and make a header */
  atom_t* bn_tlated = alloc(atom_t, ndigits + hdrlen), // 1
       * const init = make_array_header(metadata, ndigits, 0, flags); // 2
  /* put the new header in the initial section of new data */
  memcpy(bn_tlated, init, sz(atom_t, hdrlen));
  free(init); // ~2
  /* going to use base 256 */
  if (using_base256) {
    /* convert to array representation */
    uint16_t len = 0;
    /* little endian*/
    atom_t* as_digits = u64_to_b256(u64, &len, true);
    /* just copy the data into the rest of the array */
    memcpy(bn_tlated + hdrlen, as_digits, len);
    free(as_digits);
  } else {
#ifdef PREFER_CHAR_CONV
    /* here begins the string implementation */
    char* const str = alloc( char, ndigits + /* null term */ 2); // 4
    snprintf(str, MAX_U64_DIGITS + 1, "%" PRIu64 "", u64);
    for (atom_t i = 0; i < ndigits; i++) {
      bn_tlated[i + hdrlen] = (atom_t) ((unsigned) str[i] - '0');
    }
    free(str); // ~4
    /* here ends the string implementation */
#else /* ! PREFER_CHAR_CONV (default) */
    for (atom_t i = 0; i < ndigits; i++) {
      //printf("%d %" PRIu64 "\n", i + HEADER_OFFSET, u64);
      bn_tlated[i + hdrlen] = get_left_nth_digit(u64, i);
    }
#endif /* PREFER_CHAR_CONV */
  }
  return bn_tlated; // 1
} /* impl_to_digit_array_u64 */
#endif /* end of include guard: BNA_H */


#ifndef BASE256_H
#define BASE256_H
/*
  char*, bool -> atom_t*, uint16_t*
  transform a uint64_t (string) to its base 256 representation in bytes
  the value at len is changed to the new length of the representation
  if little_endian is true, then the result is reversed
  a valid pointer to a zero array is returned if
    len is NULL
    the input string is empty or NULL
  in the last case, len is set to 1 (when a valid pointer)
*/
atom_t* u64_digits_to_b256 (const char* const u64_str, uint16_t* const len, const bool little_endian) {
  if (string_is_sempty(u64_str, MAX_STR_LDBL_DIGITS) || NULL == len) {
    set_out_param(len, 1);
    return zalloc(atom_t, 1);
  }
  atom_t* const result = zalloc(atom_t, count_b256_digits_b10_digits(u64_str));
  atom_t* const as_b10 = u64_digits_to_b10(u64_str, len, false);
  (void) little_endian;
  (void) as_b10;
  puts("UNIMPLEMENTED");
  /* big endian! */
  /*
    for (uint64_t tempr = value; tempr; i++) {
      //result = realloc(result, i + 1);
      result[i] = (atom_t)   (tempr % B256_HIGH);
      tempr     = (uint64_t) floorl(tempr / B256_HIGH);
      //printf("%d, %" PRIu64 "\n", result[i], tempr);
      if (! tempr) { break; }
    }
  */
  return result;
}
/*
  char* -> atom_t*
*/
static atom_t* str_digits_to_b256 (const char* const digits, uint16_t* const len, const bool little_endian) {
  puts("UNIMPLEMENTED");
  (void) digits;
  (void) len;
  (void) little_endian;
  return NULL;
}
/*
{
  const char* const rev_digits = str_reverse(digits);
  size_t b256_place = 1, b10_place = 1; // 1-index
  atom_t* const b256_digits = alloc(atom_t, 10);
  for (size_t i = 0; rev_digits[i]; i++) {
  }
  return b256_digits;
}
*/
/*
  char*, uint16_t*, uint16_t* -> atom_t*
  transform a long double to its base 256 representation
  the integer part is first and the fractional second
  the value at len     is changed to the new entire length of the representation
  the value at int_len is changed to the new length of the integer part, from
    which can be inferred the length of the fractional part
  if little_endian is true, then the result is reversed, and the fractional part
    occurs first
  if value was 0 or len was NULL, then a valid pointer to an array of value 0
    is returned
  if len was NULL, it is not changed into a valid pointer
  the return value is always a valid pointer
*/
atom_t* ldbl_digits_to_b256 (const char* const ldbl_digits, uint16_t* const len, uint16_t* const int_len, const bool little_endian) {
  if ( string_is_sempty(ldbl_digits, MAX_STR_LDBL_DIGITS) || NULL == len || NULL == int_len) {
    set_out_param(len, 1);
    set_out_param(len, 1);
    return zalloc(atom_t, 1);
  }
  /* length of integer part before the decimal point */
  const uint16_t pre_dec = (uint16_t) strcspn(ldbl_digits, DECIMAL_SEPARATOR_STR);
  /* get the two parts of the number */
  char* const int_part  = strndup_c(ldbl_digits, pre_dec), // 1
      /* flip the significant digits */
      * const flot_part = str_reverse(ldbl_digits + pre_dec + /* skip separator */ 1); // 2
  uint16_t lhs_len = 0, rhs_len = 0;
  atom_t* const lhs_b256 = str_digits_to_b256(int_part, &lhs_len, true),
        /* note argument #3: flip the fractional digits again */
        * const rhs_b256 = str_digits_to_b256(flot_part, &rhs_len, false);
  set_out_param(len, (uint16_t) (lhs_len + rhs_len) );
  set_out_param(int_len, lhs_len);
  atom_t* const final_concat = array_concat(lhs_b256, rhs_b256, lhs_len, rhs_len);
  free(lhs_b256), free(rhs_b256);
  if (little_endian) {
    atom_t* const reversed = array_reverse(final_concat, *len);
    free(final_concat);
    return reversed;
  } else {
    return final_concat;
  }
}
/*
    vvv little endian vvv
  the following functions ONLY expect and return data with the least significant part
    at the end, rather than at the beginning
  this is in line with how numbers are usually written in both base 10 and base 2
*/
/*
  atom_t*, uint16_t -> uint64_t
  get the base 10 value out of a base 256 representation
  this function clears errno on entry, but not on exit
  0 is returned when
    digits represents the value 0
    len is 0 or greater than MAX_U64_DIGITS (usually 20)
    the value represented in digits is larger than UINT64_MAX
  in the last two cases, errno is set to ERANGE
*/
uint64_t b256_to_u64 (const atom_t* const digits, const uint16_t len) {
  if (! digits || ! len ) {
    return 0;
  }
  if (len > MAX_U64_DIGITS) { goto range_err; }
  errno = 0;
  uint64_t result = 0;
  for (int16_t i = (int16_t) (len - 1); i > -1; i--) {
    result += digits[i] * ( (uint64_t) powl(ZENZ_BASE, i) );
    if (ERANGE == errno) { goto range_err; }
  }
  return result;
  range_err:
    errno = ERANGE;
    return 0;
}
char* b256_to_u64_digits (const atom_t* const digits, const uint16_t len) {
  (void) digits;
  (void) len;
  return NULL;
}
/*
  atom_t*, uint16_t, uint16_t, uint16_t* -> char*
  convert base 256 representation of a long double to base 10 string
  an empty string is returned when
    len is 0
    int_len is greater than len
    out_int_len is not writable (i.e. NULL)
    digits is NULL
*/
char* b256_to_ldbl_digits (const atom_t* const digits, const uint16_t len, const uint16_t int_len) {
  /* prologue: check arguments */
  /* 0 is a valid int_len */
  if (NULL == digits || ! len || int_len > len) {
    return make_empty_string();
  }
  /* step 1: base 256 -> base 10 parts */
  /* copy value to a better name */
  const uint16_t int_len_orig = int_len;
  /* the length of the fractional part is from subtraction */
  const uint16_t flot_len_orig = (uint16_t) (len - int_len_orig);
  /* copy the parts (in base 256) from the original (1 2) */
  atom_t *const int_b256  = memcpy( alloc(atom_t, int_len_orig), digits, sz(atom_t, int_len_orig)),
  /* the flot part needs to be reversed so that it is little endian (descending places from left to right) */
         *const flot_b256 = array_reverse(digits + int_len_orig, flot_len_orig); // big endian -> le
  /* step 2: base 10 parts -> strings */
  /* convert / reverse the parts (3 4 5) */
  char *const int_b10     = b256_to_u64_digits(int_b256, int_len_orig),
  /* this conversion will be wrong if the flot part is left big endian */
       *const flot_b10    = b256_to_u64_digits(flot_b256, flot_len_orig), // le -> le
  /* re-reverse the flot part so that it is in the proper order */
       *const flot_b10_be = str_reverse(flot_b10); // le -> be
  /* don't need the base256 parts anymore (~1 ~2 ~4) */
  free(int_b10), free(flot_b256), free(flot_b10);
  /* step 3: strings -> string + sep + string */
  const uint16_t int_len_b10 = (uint16_t) strnlen_c(int_b10, MAX_STR_LDBL_DIGITS),
                flot_len_b10 = (uint16_t) strnlen_c(flot_b10_be, MAX_STR_LDBL_DIGITS);
  // 6
  char* const out_str = alloc(char, 2 + int_len_b10 + flot_len_b10);
  snprintf(out_str, 1U + int_len_b10 + flot_len_b10, "%s.%s", int_b10, flot_b10_be);
  // ~3 ~5
  free(int_b10), free(flot_b10_be);
  /* epilogue: finish */
  // 6 leaves scope
  return out_str;
}
atom_t* u64_to_b256 (const uint64_t value, uint16_t* const len, const bool little_endian) {
  char* const val_str = alloc(char, count_digits_u64(value));
  snprintf(val_str, MAX_U64_DIGITS, "%" PRIu64 "", value);
  atom_t* const result = u64_digits_to_b256(val_str, len, little_endian);
  free(val_str);
  return result;
}
#endif /* end of include guard: BASE256_H */


#ifndef ADDR_INTERP_H
#define ADDR_INTERP_H
// single address, multi byte
/*
  uint16_t, atom_t*, atom_t* ->
  the value at ah is changed to the high 8 bits of n
  the value at al is changed to the low  8 bits of n
  the pointers, ah and al, are never changed
*/
void samb_u16_to_twoba (const uint16_t n, atom_t* const ah, atom_t* const al) {
  *ah = (atom_t) (n >> (atom_t) 8);        // high bits
  *al = (atom_t) (n &  (atom_t) 0xFF);     // low 8 bits
}
/*
  atom_t, atom_t -> uint16_t
  the return value is composed of 8 high and 8 low bits from ah and al
  inverse of samb_u16_to_twoba
*/
uint16_t samb_twoba_to_u16 (const atom_t ah, const atom_t al) {
  // put the high 8 bits at the top and the rest at the bottom
  return (uint16_t) ( (ah << 8) | (atom_t) al);
}
/*
  atom_t[2] -> uint16_t
  apply samb_twoba_to_u16 to two consecutive values in an atom_t array
  shorthand for samb_twoba_to_u16(arr[0], arr[1])
*/
uint16_t samb_twoarray_to_u16 (const atom_t arr[static 2]) {
  return samb_twoba_to_u16(arr[0], arr[1]);
}
#endif /* end of include guard: ADDR_INTERP_H */


#ifndef MISC_UTIL_H
#define MISC_UTIL_H
// i think this is how we compare ie3-754 numbers instead of == in C
bool compare_eps (const ldbl_t a, const ldbl_t b, const ldbl_t eps) {
  return fabsl(a - b) < eps;
}
/*
  uint64_t -> atom_t
  count the number of actual digits in a base 10 number
  the result is always less than 21 because that's all you get with 64 bits
*/
atom_t count_digits_u64 (const uint64_t x) {
  return (atom_t) floor( log10f( (float) x) + 1.f );
}
/*
  atom_t*, uint16_t -> bool
*/
bool raw_is_zero (const atom_t* const digits, const uint16_t len) {
  uint16_t sum = 0;
  for (uint16_t i = 0; i < len; i++) {
    sum = (uint16_t) ((uint16_t) digits[i] + sum);
  }
  return 0 == sum;
}
/*
  char* -> atom_t
  find the beginning of the fractional part of the string representation of a
    floating point numuber
*/
atom_t find_frac_beginning (const char* const str) {
  //begin_read = (atom_t) (1U + (unsigned) pre_len);
  const atom_t
    pre_len = (atom_t) strcspn(str, DECIMAL_SEPARATOR_STR),
    len  = (atom_t) strnlen_c(str, MAX_PRIMITIVE_LDBL_DIGITS),
    diff = (atom_t) (len - pre_len);
    /* found + 1 for separator */
  if (1 == diff) {
    return pre_len;
  } else if (diff) {
    return (atom_t) (1 + pre_len);
  } else {
    return len;
  }
}
/*
  char* -> atom_t
  count the number of actual digits in a string representation of a floating
    point number
  numbers like 123. end with an implicit zero and have a fractional length of 1
*/
atom_t count_frac_digits (const char* const str) {
  const atom_t begin = find_frac_beginning(str);
  if (1 == strnlen_c(str, MAX_PRIMITIVE_LDBL_DIGITS) - begin) { return 1; }
  return (atom_t) strspn(str + begin, "0123456789");
}
/*
  uint64_t -> atom_t
  report the highest indexable digit of a number
  0 based counting; constrast with count_digits_u64
  for use with get_left_nth_digit
*/
atom_t indexable_digits_u64 (const uint64_t x) {
  return (atom_t) floor( log10f( (float) x) );
}
/*
  uint64_t, atom_t -> atom_t
  report the value in a given 10s place of a number
  indexing is 0-based and from the left side (greatest significand)
  uses a fast ( theoretically O(1) ) number-theory formula by default
  define PREFER_CHAR_CONV and you get a much slower string-based implementation
    that relies much less on floating point math
    -- it's what you asked for
*/
atom_t get_left_nth_digit (const uint64_t x, const atom_t n) {
#ifdef PREFER_CHAR_CONV
  char* const str = alloc(char, MAX_U64_DIGITS + 2);
  snprintf(str, MAX_U64_DIGITS + 1, "%" PRIu64 "", x);
  const char d = str[n];
  free(str);
  return (atom_t) ((unsigned) d - '0');
#else /* ! PREFER_CHAR_CONV */
  const ldbl_t tpow  = pow(DEC_BASE, indexable_digits_u64(x) - n),
               ldivr = ((ldbl_t) x) / tpow;
  //printf("%LG %LG\n", tpow, ldivr);
  return (atom_t) (((uint64_t) ldivr) % DEC_BASE);
#endif /* PREFER_CHAR_CONV */
}
/*
  char* -> char*
  reverse a string.
  the return value will never point to the input value, hence const* const
  the return value may be NULL but will not be an otherwise invalid pointer
*/
char* str_reverse (const char* const str) {
  if ( NULL == str ) { return NULL; }
  size_t len = strnlen_c(str, MAX_EXPORT_SIGFIGS);
  char* newp = NULL;
  if ( len < 2 ) {
    newp = strndup_c(str, len);
  } else {
    newp = alloc(char, len + 1);
    size_t i, j;
    for (i = 0, j = len - 1; i < len; i++, j--) {
      newp[i] = str[j];
      //printf("i: %zu, j: %zu, ic: %c, ij: %c\n", i, j, newp[i], str[j]);
    }
    newp[i] = '\0';
  }
  return newp;
}
/*
  float -> float
  simple base 256 logarithm
*/
float log256f (const float x) {
  return logf(x) / logf(ZENZ_BASE);
}
/*
  uint64_t -> atom_t
  count the number of digits needed in base 256 to represent x
*/
atom_t count_b256_digits_u64 (const uint64_t x) {
  return (atom_t) floorf( log256f( (float) x ) + 1.f);
}
uint16_t count_b256_digits_b10_digits (const char* const digits) {
  puts("UNIMPLEMENTED");
  uint16_t len_initial = 0;
  atom_t* const as_atoms = u64_digits_to_b10(digits, &len_initial, false);
  #define LEN_256 ((atom_t) 3)
  static const atom_t b10_256[LEN_256] = { 2, 5, 6 };
  // logn_b10(256, digits)
  uint16_t log_len = 0, log_int_len = 0;
  atom_t* const log256 = logn_b10 (b10_256, LEN_256, LEN_256, as_atoms, len_initial, len_initial, &log_len, &log_int_len);
  free(as_atoms);
  // add 1
  uint16_t len_add1 = 0, int_len_add1 = 0;
  atom_t* const add1 = succ_b10(log256, len_initial, len_initial, 0, &len_add1, &int_len_add1);
  free(log256);
  // floor
  atom_t* const floored = floor_b10(add1, len_add1, int_len_add1);
  free(add1);
  // convert to hardware
  const uint16_t final = b10_to_u16(floored, len_add1);
  free(floored);
  return final;
}
/*
  atom_t*, uint16_t -> atom_t*
  copy and reverse an array
  the return value will always be a valid unique pointer
*/
atom_t* array_reverse (const atom_t* const arr, const uint16_t len) {
  atom_t* result = alloc(atom_t, len);
  if (len) {
    for (uint16_t i = 0; i < len; i++) {
      result[i] = arr[ (len - 1) - i ];
    }
  }
  return result;
}
/*
  atom_t*, atom_t*, uint16_t, uint16_t -> atom_t*
  glue two arrays together
  always returns valid unique pointers
*/
atom_t* array_concat (const atom_t* const a, const atom_t* const b, const uint16_t a_len, const uint16_t b_len) {
  if (! (a_len + b_len) ) {
    return alloc(atom_t, 0);
  }
  else if (a_len == a_len + b_len) {
    return memcpy(alloc(atom_t, a_len), a, a_len);
  } else if (b_len == a_len + b_len) {
    return memcpy(alloc(atom_t, b_len), b, b_len);
  }
  atom_t* const res = memcpy( alloc(atom_t, a_len + b_len) , a, a_len);
  return              memcpy( res + a_len, b, b_len);
}
bool array_contains (const atom_t* const arr, const uint16_t len, const atom_t value) {
  for (size_t i = 0; i < len; i++) {
    if (value == arr[i]) { return true; }
  }
  return false;
}
/*
  -> char*
  return an empty (0-length) string
*/
char* make_empty_string (void) {
  return zalloc(char, 1);
}
/*
  atom_t*, uint16_t, uint16_t, atom_t... -> uint16_t
  strspn / strcspn but for arrays
  return the length of the initial section of arr which consists only of values
    in accept_only
  see strspn(3) and strcspn(3)
*/
uint16_t array_span (const atom_t* arr, const uint16_t arr_len, const bool accept, const atom_t* const vals, const uint16_t vals_len) {
  uint16_t i = 0;
  for (; i < arr_len; i++) {
    if (accept == !array_contains(vals, vals_len, arr[i])) {
      break;
    }
  }
  return i;
}
/*
  atom_t* -> atom_t*
  remove insignificant trailing zeroes from an array of digits in any base
    since the representation of literal 0 is constant across radicies
*/
atom_t* array_trim_trailing_zeroes (const atom_t* const bn) {
  const atom_t hdrlen = bna_header_offset(bn);
  const bool   is_big = bna_is_big(bn);
  const uint16_t
    //len     = (uint16_t) bna_real_len(bn),
    int_len = bna_int_len(bn),
    frc_len = bna_frac_len(bn);
  //atom_t const * rev_cpy = array_reverse(bn, len);
  //uint16_t consec_zeroes = array_spn(rev_cpy, len, 1, 0);
  puts("UNIMPLEMENTED");
  (void) hdrlen;
  (void) is_big;
  (void) int_len;
  (void) frc_len;
  //(void) consec_zeroes;
  return NULL;
}
/*
  atom_t* -> atom_t*
  remove insignificant leading zeroes from an array of digits in any base
*/
atom_t* array_trim_leading_zeroes_simple (const atom_t* const bn, const uint16_t len) {
  atom_t* const z = zalloc(atom_t, 1);
  const uint16_t count_leading_zeroes = array_span(bn, len, true, z, 1);
  free(z);
  const uint16_t nonzeroes = (uint16_t) (len - count_leading_zeroes);
  return memcpy(alloc(atom_t, nonzeroes), bn + count_leading_zeroes, nonzeroes);
}
char* strndup_c (const char* const s, size_t const n) {
  const size_t len = strnlen_c(s, n);
  char* const news = alloc(char, len + 1);
  if (NULL == news) {
    return NULL;
  }
  news[len] = '\0';
  return memcpy(news, s, len);
}
size_t strnlen_c (const char* const s, const size_t maxsize) {
  if (NULL == s) {
      return 0;
  }
  size_t i = 0;
  for (; i < maxsize; i++) {
    if ('\0' == s[i]) { return i; }
  }
  return i;
}
size_t str_count (const char* const str, const char find) {
  size_t ocur = 0;
  const size_t len = strnlen_c(str, MAX_STR_LDBL_DIGITS);
  for (size_t i = 0; i < len; i++) {
    if (find == str[i]) { ++ocur; }
  }
  return ocur;
}
void say_atom_t_ptr (const atom_t* const data, const uint16_t len) {
  printf("atom_t ptr %p+%d: ", (const void* const) data, len);
  for (uint16_t i = 0; i < len; i++) {
    printf("%d ", data[i]);
  }
  printf(";\n");
}
#endif /* end of include guard: MISC_UTIL_H */

#endif
