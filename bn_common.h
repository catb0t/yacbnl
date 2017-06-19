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

#ifndef BN_COMMON_H
#define BN_COMMON_H

#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <inttypes.h>

#define HEADER_OFFSET     ((atom_t) 4)
#define HEADER_OFFSET_BIG ((atom_t) 6)

#ifndef MAX_SIGFIGS
  #define MAX_SIGFIGS   ((UINT8_MAX * 2) - (HEADER_OFFSET * 2))
#endif

#ifndef MAX_SIGFIGS_BIG
  #define MAX_SIGFIGS_BIG ((UINT16_MAX * 2) - (HEADER_OFFSET_BIG * 2))
#endif

#ifndef PRIMITIVE_PRECISION
  #define PRIMITIVE_PRECISION 24
#endif

#ifdef PREFER_CHAR_CONV
  #pragma message("preferring slower O(n) char operations over O(1) floating point math")
#endif

/* TYPEDEFS */

typedef long double ldbl_t;
typedef uint8_t     atom_t;

typedef struct st_bignum_t {

  /*
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

#define        sz(n, type) ( ((size_t) n) * (sizeof (type)) )
#define  alloc(size, type) malloc(( (size_t) size) * sizeof (type))
#define zalloc(size, type) calloc(( (size_t) size),  sizeof (type))
#define  bna_real_len(bna) ((bna)[0] + (bna)[1] + HEADER_OFFSET)
#define     macrogetval(x) #x
#define       stringify(x) macrogetval(x)


/* composable flags */
/* array types */
#define TYP_BIG  0x01 /* this array is big (2 byte addressing mode) */
#define TYP_ZENZ 0x02 /* array uses base 256 rather than base 10 (from the word zenzizenzizenzic) */

/* these apply to number values themselves */
#define FL_SIGN 0x01
#define FL_NAN  0x02
#define FL_INF  0x04

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
bool            compare_eps (const ldbl_t a, const ldbl_t b, const ldbl_t eps);
atom_t     count_digits_u64 (const uint64_t x);
atom_t indexable_digits_u64 (const uint64_t x);
atom_t   get_left_nth_digit (const uint64_t x, const atom_t n);
atom_t    count_frac_digits (const char* const str);
atom_t  find_frac_beginning (const char* const str);

/* bignum_t */
bignum_t* bignum_ctor (const ldbl_t ldbl, const uint64_t u64, const atom_t flags, const bignum_t * const * const opt_vals);
bignum_t* bignum_copy (const bignum_t* const bn, const bool no_recurse_optionals);

/* array creation */
atom_t* to_digit_array (const ldbl_t ldbl_in, const uint64_t u64, const atom_t value_flags, const atom_t metadata);

/* 2 byte addressing stuff */
void     u16_to_twoba (const uint16_t n, atom_t* const ah, atom_t* const al);
uint16_t twoba_to_u16 (const atom_t ah, const atom_t al);

/* base 256 conversions */
uint64_t b256_to_u64_digits (const atom_t* const digits, const uint16_t len);
atom_t*       array_reverse (const atom_t* const arr, const uint16_t len);
atom_t*  u64_digits_to_b256 (const uint64_t value, uint16_t* const len);

#endif /* end of include guard: BN_COMMON_H */
