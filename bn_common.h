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
#include <inttypes.h>

#define HEADER_OFFSET ((atom_t) 3)

#ifndef MAX_SIGFIGS
  #define MAX_SIGFIGS   ((UINT8_MAX * 2) - (HEADER_OFFSET * 2))
#endif

#ifdef PREFER_CHAR_CONV
  #pragma message("preferring slower O(n) char operations over O(1) floating point math")
#endif

#define        sz(n, type) ( ((size_t) n) * (sizeof (type)) )
#define  alloc(size, type) malloc(( (size_t) size) * sizeof (type))
#define zalloc(size, type) calloc(( (size_t) size),  sizeof (type))
#define bna_real_len(bna) ((bna)[0] + (bna)[1] + HEADER_OFFSET)

/* composable */
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

typedef long double ldbl_t;
typedef uint8_t     atom_t;

typedef struct st_bignum_t {

  /*
    value + 0  = number of following integral digits
    value + 1  = number of fractional digits which follow that
    value + 2  = flags, FL_NONE or any OR-combination of FL_INF, FL_NAN and FL_SIGN

    4 3 0 3 4 5 6 6 7 8 = 3456.678
  */
  atom_t
    * value, // the main value
    * imgry, // imaginary part
    * fracl, // fractional part (numerator / denom)
    * vextn; // reference this if bna_real_len(x) >= 255
  struct st_bignum_t* expt;
} bignum_t;

bignum_t* bignum_ctor (const ldbl_t ldbl, const uint64_t u64, const atom_t flags, const bignum_t * const * const opt_vals);
bignum_t* bignum_copy (const bignum_t* const bn, const bool no_recurse_optionals);

atom_t     count_digits_u64 (const uint64_t x);
atom_t indexable_digits_u64 (const uint64_t x);
atom_t   get_left_nth_digit (const uint64_t x, const atom_t n);
atom_t    count_frac_digits (const char* const str);
atom_t  find_frac_beginning (const char* const str);
atom_t*         to_bn_array (const ldbl_t ldbl, const uint64_t u64, const atom_t flags);

bool compare_eps (const ldbl_t a, const ldbl_t b, const ldbl_t eps);

#endif /* end of include guard: BN_COMMON_H */
