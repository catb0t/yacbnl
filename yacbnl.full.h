#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
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


#define HEADER_OFFSET     ((atom_t) 4)
#define HEADER_OFFSET_BIG ((atom_t) 6)

#ifndef MAX_SIGFIGS
  #define MAX_SIGFIGS   ((UINT8_MAX * 2) - (HEADER_OFFSET * 2))
#endif

#ifndef MAX_SIGFIGS_BIG
  #define MAX_SIGFIGS_BIG ((UINT16_MAX * 2) - (HEADER_OFFSET_BIG * 2))
#endif

#ifndef MAX_DOUBLE_KEPT_FIGS
  #define MAX_DOUBLE_KEPT_FIGS 200
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

#define B256_HIGH 0x100
#define B10_HIGH  0xA

/* composable flags */
/* array types */
#define TYP_NONE  0x0
#define TYP_BIG   0x01 /* this array is big (2 byte addressing mode) */
#define TYP_ZENZ  0x02 /* array uses base 256 rather than base 10 (from the word zenzizenzizenzic) */
#define TYP_OVERF 0x04 /* this array's intended value would overflow this array; combine its data with another */
#define TYP_EXTN  0x08 /* this array is the extension of another array whose value is overflowed */

/* these apply to number values themselves */
#define FL_NONE 0x0
#define FL_SIGN 0x01
#define FL_NAN  0x02
#define FL_INF  0x04

#define    meta_is_base256(metadata) (metadata & TYP_ZENZ)
#define        meta_is_big(metadata) (metadata & TYP_BIG)
#define meta_header_offset(metadata) (meta_is_big(metadata) ? HEADER_OFFSET_BIG : HEADER_OFFSET)

#define    bna_is_base256(bna) (meta_is_base256(bna[0]))
#define        bna_is_big(bna) (meta_is_big(bna[0]))
#define bna_header_offset(bna) (meta_header_offset(bna[0]))
#define      bna_real_len(bna) (bna_is_big(bna) ? (samb_twoarray_to_u16((bna) + 1) + samb_twoarray_to_u16((bna) + 3) + HEADER_OFFSET_BIG) : ((bna)[1] + (bna)[2] + HEADER_OFFSET) )
#define         bna_flags(bna) ((bna)[bna_header_offset((bna)) - 1])

#define        sz(n, type) ( ((size_t) n) * (sizeof (type)) )
#define  alloc(size, type) malloc(( (size_t) size) * sizeof (type))
#define zalloc(size, type) calloc(( (size_t) size),  sizeof (type))
#define     macrogetval(x) #x
#define       stringify(x) macrogetval(x)

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
atom_t    get_left_nth_digit (const uint64_t x, const atom_t n);
atom_t     count_frac_digits (const char* const str);
atom_t   find_frac_beginning (const char* const str);
atom_t*         array_concat (const atom_t* const a, const atom_t* const b, const uint16_t a_len, const uint16_t b_len);
atom_t*        array_reverse (const atom_t* const arr, const uint16_t len);
char*            str_reverse (const char* const str);
char*      make_empty_string (void);

atom_t* array_trim_trailing_zeroes (const atom_t* const bn);
atom_t*  array_trim_leading_zeroes (const atom_t* const bn);


/* bignum_t */
bignum_t* bignum_ctor (const ldbl_t ldbl, const uint64_t u64, const atom_t flags, const bignum_t * const * const opt_vals);
bignum_t* bignum_copy (const bignum_t* const bn, const bool no_recurse_optionals);

/* array creation */
atom_t* to_digit_array (const ldbl_t ldbl_in, const uint64_t u64, const atom_t value_flags, const atom_t metadata);

/* 2 byte addressing stuff */
void        samb_u16_to_twoba (const uint16_t n, atom_t* const ah, atom_t* const al);
uint16_t    samb_twoba_to_u16 (const atom_t ah, const atom_t al);
uint16_t samb_twoarray_to_u16 (const atom_t arr[static 2]);

/* base 256 conversions */
char*    b256_to_ldbl_digits (const atom_t* const digits, const uint16_t len, const uint16_t int_len, uint16_t* const out_int_len);
uint64_t  b256_to_u64_digits (const atom_t* const digits, const uint16_t len);
atom_t*  ldbl_digits_to_b256 (const char* const ldbl_digits, uint16_t* const len, uint16_t* const int_len);
atom_t*   u64_digits_to_b256 (const uint64_t value, uint16_t* const len, const bool little_endian);

#endif /* end of include guard: BN_COMMON_H */
#ifndef MISC_UTIL_H
#define MISC_UTIL_H


// i think this is how we compare ie3-754 numbers instead of == in C
bool compare_eps (const ldbl_t a, const ldbl_t b, const ldbl_t eps) {
  return fabsl(a - b) < eps;
}

/*
  uint64_t -> atom_t

  count the number of actual digits in a base 10 number.
  the result is always less than 21 because that's all you get with 64 bits
*/
atom_t count_digits_u64 (const uint64_t x) {
  return (atom_t) floor( log10f( (float) x) + 1.f );
}

/*
  char* -> atom_t

  find the beginning of the fractional part of the string representation of a
  floating point numuber
*/
atom_t find_frac_beginning (const char* const str) {
  const atom_t pre_len = (atom_t) strcspn(str, ".");
            //begin_read = (atom_t) (1U + (unsigned) pre_len);
            /* + 1 for separator */
  return (atom_t) (1U + (unsigned) pre_len);
}

/*
  char* -> atom_t

  count the number of actual digits in a string representation of a floating
  point number
*/
atom_t count_frac_digits (const char* const str) {
  return (atom_t) strspn(str + find_frac_beginning(str), "0123456789");
}

/*
  uint64_t -> atom_t

  report the highest indexable digit of a number; for use with get_left_nth_digit.
  0 based counting; constrast with count_digits_u64.
*/
atom_t indexable_digits_u64 (const uint64_t x) {
  return (atom_t) floor( log10f( (float) x) );
}

/*
  uint64_t, atom_t -> atom_t

  report the value in a given 10s place of a number
  indexing is 0-based and from the left side (greatest significand).

  uses a fast ( theoretically O(1) ) number-theory formula by default.

  define PREFER_CHAR_CONV and you get a much slower, string-based implementation,
  that relies much less on floating point math.
  it's what you asked for.
*/
atom_t get_left_nth_digit (const uint64_t x, const atom_t n) {
#ifdef PREFER_CHAR_CONV

  char* const str = alloc(22, char);
  snprintf(str, 21, "%" PRIu64 "", x);

  char d = str[n];
  free(str);
  return (atom_t) ((unsigned) d - '0');

#else /* ! PREFER_CHAR_CONV */

  const ldbl_t tpow  = pow(10, indexable_digits_u64(x) - n),
               ldivr = ((ldbl_t) x) / tpow;

  //printf("%LG %LG\n", tpow, ldivr);
  return (atom_t) (((uint64_t) ldivr) % 10);

#endif /* PREFER_CHAR_CONV */
}

char* str_reverse (const char* const str) {
  if ( NULL == str ) { return NULL; }

  size_t len = strnlen(str, MAX_SIGFIGS);
  char* newp;

  if ( len < 2 ) {
    newp = strndup(str, len);

  } else {
    newp = alloc(len + 1, char);

    size_t i, j;
    for (i = 0, j = len - 1; i < len; i++, j--) {
      newp[i] = str[j];
      //printf("i: %zu, j: %zu, ic: %c, ij: %c\n", i, j, newp[i], str[j]);
    }

    newp[i] = '\0';
  }

  return newp;
}

float log256f (const float x) {
  return logf(x) / logf(256);
}

atom_t count_b256_digits_u64 (const uint64_t x) {
  return (atom_t) floorf( log256f( (float) x ) + 1.f);
}

atom_t* array_reverse (const atom_t* const arr, const uint16_t len) {

  atom_t* result = zalloc(len, atom_t);

  if (len) {
    for (uint16_t i = 0; i < len; i++) {
      result[i] = arr[ i - len ];
    }
  }

  return result;
}

atom_t* array_concat (const atom_t* const a, const atom_t* const b, const uint16_t a_len, const uint16_t b_len) {
  if (! (a_len + b_len) ) {
    return alloc(0, atom_t);
  }

  else if (a_len == a_len + b_len) {
    return memcpy(alloc(a_len, atom_t), a, a_len);

  } else if (b_len == a_len + b_len) {
    return memcpy(alloc(b_len, atom_t), b, b_len);
  }

  atom_t* const res = memcpy( alloc(a_len + b_len, atom_t) , a, a_len);
  return              memcpy( res + a_len, b, b_len);
}

char* make_empty_string (void) {
  return zalloc(1, char);
}

atom_t* array_trim_trailing_zeroes (const atom_t* const bn) {
  (void) bn;
  return NULL;
}

atom_t* array_trim_leading_zeroes (const atom_t* const bn) {
  (void) bn;
  return NULL;
}

#endif /* end of include guard: MISC_UTIL_H */
#ifndef ADDR_INTERP_H
#define ADDR_INTERP_H


void samb_u16_to_twoba (const uint16_t n, atom_t* const ah, atom_t* const al) {
  *ah = (atom_t) (n >> (atom_t) 8);// high bits
  *al = (atom_t) (n &  (atom_t) 0xFF);     // low 8 bits
}

uint16_t samb_twoba_to_u16 (const atom_t ah, const atom_t al) {
  // put the high 8 bits at the top and the rest at the bottom
  return (uint16_t) ( (ah << 8) | (atom_t) al);
}

uint16_t samb_twoarray_to_u16 (const atom_t arr[static 2]) {
  return samb_twoba_to_u16(arr[0], arr[1]);
}

/*atom_t* u64_to_octba (const uint64_t n) {

  return NULL;
}

uint64_t octba_to_u64 (const atom_t* const bytes) {
  uint64_t result = 0;

  for (size_t i = 0; i < CHAR_BIT; i++) {
    result |= (uint64_t) bytes[i] << ( (CHAR_BIT - (i + 1) ) * CHAR_BIT );
  }

  return result;
}
*/
#endif /* end of include guard: ADDR_INTERP_H */
#ifndef BASE256_H
#define BASE256_H


atom_t* u64_digits_to_b256 (const uint64_t value, uint16_t* const len, const bool little_endian) {

  if (! value || NULL == len) {
    return zalloc(1, atom_t);
  }

  atom_t* const result = zalloc(count_b256_digits_u64(value), atom_t);

  uint16_t i = 0;

  /* big endian! */
  for (uint64_t tempr = value; tempr; i++) {
    //result = realloc(result, i + 1);

    result[i] = (atom_t)   (tempr % B256_HIGH);
    tempr     = (uint64_t) floorl(tempr / B256_HIGH);
    //printf("%d, %" PRIu64 "\n", result[i], tempr);
    if (! tempr) { break; }
  }

  *len = (uint16_t) (i + 1);

  if (little_endian) {
    atom_t* const reversed = array_reverse(result, *len);
    free(result);

    return reversed;
  } else {
    return result;
  }
}

atom_t* ldbl_digits_to_b256 (const char* const ldbl_digits, uint16_t* const len, uint16_t* const int_len) {

  /* integer aprt before the decimal point */
  const uint16_t pre_dec = (uint16_t) strcspn(ldbl_digits, ".");

  char* const int_part  = strndup(ldbl_digits, pre_dec),
      /* flip the significant digits */
      * const flot_part = str_reverse(ldbl_digits + pre_dec + 1); /* skip separator */

  /* convert the strings to numbers */
  const uint64_t lhs = strtoull(int_part,  NULL, B10_HIGH),
                 rhs = strtoull(flot_part, NULL, B10_HIGH);

  free(int_part), free(flot_part);

  uint16_t lhs_len, rhs_len;
  atom_t* const lhs_b256 = u64_digits_to_b256(lhs, &lhs_len, true),
        /* note argument #3: flip the fractional digits again */
        * const rhs_b256 = u64_digits_to_b256(rhs, &rhs_len, false);

  *len     = (uint16_t) (lhs_len + rhs_len);
  *int_len = lhs_len;

  atom_t* const final_concat = array_concat(lhs_b256, rhs_b256, lhs_len, rhs_len);

  free(lhs_b256), free(rhs_b256);

  return final_concat;
}

/* vvv little endian vvv */

uint64_t b256_to_u64_digits (const atom_t* const digits, const uint16_t len) {

  if (! len) {
    return 0;
  }

  uint64_t result = 0;

  //for (uint16_t i = 0; i < len; i++) {
  for (uint16_t i = len; i != 0; i--) {
    result += digits[i] * ( (uint64_t) powl(256, i) );
  }

  return result;
}

char* b256_to_ldbl_digits (const atom_t* const digits, const uint16_t len, const uint16_t int_len, uint16_t* const out_int_len) {

  if (! len) {
    return make_empty_string();
  }

  const uint16_t flot_len = (uint16_t) (len - int_len);

  atom_t * const int_part  = memcpy(alloc(int_len, atom_t), digits, sz(int_len, atom_t)),
         * const flot_part = memcpy(alloc(len - int_len, atom_t), digits + int_len, sz(len - int_len, atom_t));

  const uint64_t int_val  = b256_to_u64_digits(int_part, int_len),
                 flot_val = b256_to_u64_digits(flot_part, flot_len);
  free(int_part), free(flot_part);

  const uint16_t int_len10  = count_digits_u64(int_val),
                 flot_len10 = count_digits_u64(flot_val);

  char * const flot_str = alloc(flot_len10, atom_t),
       * const finalval = alloc(int_len10 + flot_len10 + 1, atom_t);

  snprintf(flot_str, (uint16_t) (flot_len10 + 1), "%" PRIu64 "", flot_val);

  char* const flot_str_be = str_reverse(flot_str);
  free(flot_str);

  snprintf(finalval, (uint16_t) (int_len10 + flot_len10 + /* null + sep */ 2), "%" PRIu64 ".%s", int_val, flot_str_be);

  *out_int_len = count_digits_u64(int_val);

  return finalval;
}

#endif /* end of include guard: BASE256_H */
#ifndef BNA_H
#define BNA_H


static atom_t* impl_to_digit_array_ldbl (const ldbl_t ldbl,  const atom_t metadata, const atom_t flags);
static atom_t*  impl_to_digit_array_u64 (const uint64_t u64, const atom_t metadata, const atom_t flags);

/* create the first 4 or 6 bytes at the beginning of every digit array */
static atom_t* make_array_header (const atom_t metadata, const uint16_t int_digits, const uint16_t flot_digits, const atom_t flags) {

  const atom_t  hdrlen = meta_header_offset(metadata);
  atom_t* const header = zalloc(hdrlen, atom_t);

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
    memcpy(header + 1, &lens, sz(4, atom_t) );

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
  const ldbl_t ldbl   = ldbl_in < 0 ? fabsl(ldbl_in) : ldbl_in;

  // not zero so we'll use it
  if ( ! compare_eps(ldbl, 0.f, 1e-11) ) {

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

static atom_t* impl_to_digit_array_ldbl (const ldbl_t ldbl, const atom_t metadata, const atom_t flags) {

  /* important metadata flags */
  const bool is_base256 = meta_is_base256(metadata),
                 is_big = meta_is_big(metadata);

  /* length of the entire header section */
  const atom_t hdrlen    = meta_header_offset(metadata);
  /* upper bound on significant figures we can store in one array */
  const uint32_t sigfigs = is_big ? MAX_SIGFIGS_BIG : MAX_SIGFIGS;

  /* put the entire value into a string, which may have trailing zeroes */
  char * const fullstr  = alloc(sigfigs + 3 /* separator + null */, char);
  snprintf(fullstr, sigfigs + 2 /* sep */, "%Lf", ldbl);

  const atom_t
    // already in fullstr form but the question is, which has lower time complexity
#ifdef PREFER_CHAR_CONV
    nint_digits  = (atom_t) strcspn(fullstr, "."),
#else /* ! PREFER_CHAR_CONV */
    nint_digits  = count_digits_u64( (uint64_t) floorl(ldbl) ),
#endif /* PREFER_CHAR_CONV */
    nflot_digits = count_frac_digits(fullstr);

  printf("digits: %d %d\n", nint_digits, nflot_digits);

  atom_t * bn_tlated  = alloc(nint_digits + nflot_digits + hdrlen, atom_t),
         * const init = make_array_header(metadata, nint_digits, nflot_digits, flags);

  printf("!!! DBGPRN %s\n", fullstr);

  memcpy(bn_tlated, init, sz(hdrlen, atom_t));

  if (is_base256) {

  } else {

      /* going to do the integral component */
#ifdef PREFER_CHAR_CONV
    char* const integ_str = strndup(fullstr, nint_digits);
    for (atom_t i = 0; i < nint_digits; i++) {
      bn_tlated[i + hdrlen] = (atom_t) ((unsigned) integ_str[i] - '0');
    }
    free(integ_str);
#else /* ! PREFER_CHAR_CONV */
    for (atom_t i = 0; i < nint_digits; i++) {
      bn_tlated[i + hdrlen] = get_left_nth_digit( (uint64_t) floorl(ldbl), i);
    }
#endif /* PREFER_CHAR_CONV */

    /* going to do the fractional component */
    // must be done with string
    const char* const frac_str = fullstr + find_frac_beginning(fullstr);

    for (size_t i = 0; i < nflot_digits; i++) {
      bn_tlated[i + hdrlen + nint_digits] = (atom_t) ((unsigned) frac_str[i] - '0');
    }

  }

  free(init), free(fullstr);
  return bn_tlated;
} /* impl_to_digit_array_ldbl */

static atom_t* impl_to_digit_array_u64 (const uint64_t u64, const atom_t metadata, const atom_t flags) {

  const bool using_base256 = metadata & TYP_ZENZ;

  const atom_t ndigits = count_digits_u64(u64),
                hdrlen = meta_header_offset(metadata);


  atom_t* bn_tlated = alloc(ndigits + hdrlen, atom_t),
       * const init = make_array_header(metadata, ndigits, 0, flags);

  memcpy(bn_tlated, init, sz(hdrlen, atom_t));
  free(init);

  if (using_base256) {
    char* const str = alloc( ndigits + /* null term */ 2, char);
    snprintf(str, 21, "%" PRIu64 "", u64);

    free(str);

  } else {

#ifdef PREFER_CHAR_CONV

    /* here begins the string implementation */
    char* const str = alloc( ndigits + /* null term */ 2, char);
    snprintf(str, 21, "%" PRIu64 "", u64);

    for (atom_t i = 0; i < ndigits; i++) {
      bn_tlated[i + hdrlen] = (atom_t) ((unsigned) str[i] - '0');
    }
    free(str);
    /* here ends the string implementation */

#else /* ! PREFER_CHAR_CONV (default) */

    for (atom_t i = 0; i < ndigits; i++) {
      //printf("%d %" PRIu64 "\n", i + HEADER_OFFSET, u64);
      bn_tlated[i + hdrlen] = get_left_nth_digit(u64, i);
    }

#endif /* PREFER_CHAR_CONV */

  }

  return bn_tlated;
} /* impl_to_digit_array_u64 */

/* !! BIG !! */


/*static atom_t* impl_to_dec_bigarray_u64 (const uint64_t u64, const atom_t flags) {
  return NULL;
}

static atom_t* impl_to_dec_bigarray_ldbl (const ldbl_t ldbl, const atom_t flags) {
  return NULL;
}
*/

#endif /* end of include guard: BNA_H */

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

  bignum_t st_bn = {
    .value = to_digit_array(ldbl, u64, flags, 0),
    .imgry = cx
      ? memcpy(
          alloc( bna_real_len(opt_vals[0]->value), atom_t),
          opt_vals[0]->value,
          sz( bna_real_len(opt_vals[0]->value), atom_t)
        )
      : zalloc(HEADER_OFFSET, atom_t),

    .fracl = fr
      ? memcpy(
        alloc( bna_real_len(opt_vals[1]->value), atom_t),
        opt_vals[1]->value,
        sz( bna_real_len(opt_vals[1]->value), atom_t)
      )
      : zalloc(HEADER_OFFSET, atom_t),

    .expt = ex
      ? bignum_copy(opt_vals[2], true)
      : bignum_ctor(0, 0, 0, NULL)
  };

  bignum_t* hp_bn = memcpy(alloc(1, bignum_t), &st_bn, sizeof(bignum_t) );

  return hp_bn;
}

/*
  bignum_t* -> bignum_t*

  deep copy a bignum_t's properties but not its identity.
*/
bignum_t* bignum_copy (const bignum_t* const bn, const bool no_recurse_optionals) {

  if (NULL == bn) {
    return zalloc(1, bignum_t);
  }

  (void) no_recurse_optionals;

  return NULL;
}

#endif /* end of include guard: BIGNUM_H */
