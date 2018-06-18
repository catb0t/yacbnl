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
uint16_t samb_twoarray_to_u16 (const atom_t arr[2]);

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
#define  alloc(type, size) ((type*) malloc(( (size_t) size) * sizeof (type)))
// same, but cleans (zeroes) the bytes with calloc
#define zalloc(type, size) ((type*) calloc(( (size_t) size),  sizeof (type)))
#define     macrogetval(x) #x
#define       stringify(x) macrogetval(x)

#ifndef set_out_param
  #define set_out_param(name, value) do { if ( NULL != (name) ){ *name = value; } } while(0)
#endif

#ifndef string_is_sempty
  #define string_is_sempty(str, n) (NULL == str || ! strnlen_c(str, n))
#endif

#ifndef LOG_ITERATIONS
  #define LOG_ITERATIONS 1
#endif

#ifndef log_b10
  #define log_b10(a, b, c, d, e) impl_log_b10(a, b, c, d, e, LOG_ITERATIONS)
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
void _say_atom_t_ptr (const atom_t* const data, const uint16_t len);

#ifdef DEBUG
  #ifndef say_atom_t_ptr
    #define say_atom_t_ptr(data, len) do{printf("%s ", #data); _say_atom_t_ptr(data, len);}while(0)
  #endif
#else
  #ifndef say_atom_t_ptr
    #define say_atom_t_ptr(data, len)
  #endif
#endif

uint16_t array_span (const atom_t* arr, const uint16_t arr_len, const bool accept, const atom_t* const vals, const uint16_t vals_len);

bool   array_contains (const atom_t* const arr, const uint16_t len, const atom_t value);
atom_t*  array_concat (const atom_t* const a, const uint16_t a_len, const atom_t* const b, const uint16_t b_len);
atom_t* array_reverse (const atom_t* const arr, const uint16_t len);
atom_t*    array_copy (const atom_t* const a, const uint16_t len);

atom_t*         array_trim_leading_zeroes (const atom_t* const bn);
atom_t*        array_trim_trailing_zeroes (const atom_t* const bn);
atom_t* array_trim_trailing_zeroes_simple (const atom_t* const bn, const uint16_t len, uint16_t* const out_len);
atom_t*  array_trim_leading_zeroes_simple (const atom_t* const bn, const uint16_t len, uint16_t* const out_len);


/* bignum_t */
bignum_t* bignum_ctor (const ldbl_t ldbl, const uint64_t u64, const atom_t flags, const bignum_t * const * const opt_vals);
bignum_t* bignum_copy (const bignum_t* const bn, const bool no_recurse_optionals);

/* array creation */
atom_t* make_array_header (const atom_t metadata, const uint16_t int_digits, const uint16_t flot_digits, const atom_t flags);
atom_t* to_digit_array (const ldbl_t ldbl_in, const uint64_t u64, const atom_t value_flags, const atom_t metadata);

/* 2 byte addressing stuff */
void        samb_u16_to_twoba (const uint16_t n, atom_t* const ah, atom_t* const al);
uint16_t    samb_twoba_to_u16 (const atom_t ah, const atom_t al);
//uint16_t samb_twoarray_to_u16 (const atom_t arr[static 2]);

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
atom_t* impl_log_b10(const atom_t* const n, const uint16_t n_len, const uint16_t n_int_len, uint16_t* const out_len, uint16_t* const out_int_len, const uint16_t iterations);
// log base n of x
atom_t* logn_b10 (const atom_t* const base, const uint16_t base_len, const uint16_t base_int_len, const atom_t* const n, const uint16_t n_len, const uint16_t n_int_len, uint16_t* const out_len, uint16_t* const out_int_len);
atom_t* add_b10 (const atom_t* const a, const uint16_t a_len, const uint16_t a_int_len, const atom_t* const b, const uint16_t b_len, const uint16_t b_int_len, uint16_t* const out_len, uint16_t* const out_int_len);
atom_t* sub_b10 (const atom_t* const a, const uint16_t a_len, const uint16_t a_int_len, const atom_t* const b, const uint16_t b_len, const uint16_t b_int_len, uint16_t* const out_len, uint16_t* const out_int_len);
atom_t* mul_b10 (const atom_t* const a, const uint16_t a_len, const uint16_t a_int_len, const atom_t* const b, const uint16_t b_len, const uint16_t b_int_len, uint16_t* const out_len, uint16_t* const out_int_len);
atom_t* div_b10 (const atom_t* const a, const uint16_t a_len, const uint16_t a_int_len, const atom_t* const b, const uint16_t b_len, const uint16_t b_int_len, uint16_t* const out_len, uint16_t* const out_int_len);
atom_t* pow_b10 (const atom_t* const a, const uint16_t a_len, const uint16_t a_int_len, const atom_t* const b, const uint16_t b_len, const uint16_t b_int_len, uint16_t* const out_len, uint16_t* const out_int_len);

atom_t* times2_b10 (const atom_t* const n, const uint16_t len, const uint16_t int_len, uint16_t* const out_len, uint16_t* const out_int_len);
atom_t* sq_b10 (const atom_t* const n, const uint16_t len, const uint16_t int_len, uint16_t* const out_len, uint16_t* const out_int_len);
atom_t* recip_b10 (const atom_t* const n, const uint16_t len, const uint16_t int_len, uint16_t* const out_len, uint16_t* const out_int_len);
atom_t* floor_b10 (const atom_t* const n, const uint16_t len, const uint16_t int_len, uint16_t* const out_len, uint16_t* const out_int_len);

#endif /* end of include guard: BN_COMMON_H */
