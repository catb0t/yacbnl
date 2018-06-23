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

#ifndef YACBNL_H
#define YACBNL_H
                                 
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
  
 #define HEADER_OFFSET ((atom_t) 4)
 #define HEADER_OFFSET_BIG ((atom_t) 6)
 
 #define DEC_BASE 10
 #define ZENZ_BASE 256
 #define COMPARE_EPS 1e-11 
 #define CHAR_DIGIT_DIFF ((uint8_t) 48) 
  
 #ifndef MAX_EXPORT_SIGFIGS
 #define MAX_EXPORT_SIGFIGS ((UINT8_MAX * 2) - (HEADER_OFFSET * 2))
 #endif
  
 #ifndef MAX_EXPORT_SIGFIGS_BIG
 #define MAX_EXPORT_SIGFIGS_BIG ((UINT16_MAX * 2) - (HEADER_OFFSET_BIG * 2))
 #endif
   
    
 #ifndef MAX_PRIMITIVE_LDBL_DIGITS
 #define MAX_PRIMITIVE_LDBL_DIGITS 50 
 #endif
  
 #ifndef MAX_U64_DIGITS
 #define MAX_U64_DIGITS 20
 #endif
  
 #ifndef MAX_STR_LDBL_DIGITS
 #define MAX_STR_LDBL_DIGITS 10000
 #endif
 
 #ifndef DECIMAL_SEPARATOR_STR
 #define DECIMAL_SEPARATOR_STR "."
 #endif
     
 #ifdef PREFER_CHAR_CONV
 #pragma message("preferring slower O(n) char operations over O(1) floating point math")
 #endif
    typedef long double ldbl_t; typedef uint8_t atom_t;  typedef struct st_bignum_t{                                atom_t *value, *imgry, *fracl, *vextn; struct st_bignum_t*expt;  }bignum_t;  
 #define B256_HIGH 0x100
 #define B10_HIGH 0xA
   
 #define TYP_NONE 0x0 
 #define TYP_BIG 0x01 
 #define TYP_ZENZ 0x02 
 #define TYP_OVERF 0x04 
 #define TYP_EXTN 0x08 
     
 #define FL_NONE 0x0 
 #define FL_SIGN 0x01 
 #define FL_NAN 0x02 
 #define FL_INF 0x04 
          
 #define meta_is_base256(metadata) (metadata & TYP_ZENZ)
 
 #define meta_is_big(metadata) (metadata & TYP_BIG)
 
 #define meta_header_offset(metadata) (meta_is_big(metadata) ? HEADER_OFFSET_BIG : HEADER_OFFSET)
     
 #define bna_is_base256(bna) (meta_is_base256(bna[0]))
 
 #define bna_is_big(bna) (meta_is_big(bna[0]))
 
 #define bna_header_offset(bna) (meta_header_offset(bna[0]))
 
 #define bna_real_len(bna) (bna_int_len(bna) + bna_frac_len(bna) + bna_header_offset(bna))
 
 #define bna_flags(bna) ((bna)[bna_header_offset((bna)) - 1])
 uint16_t samb_twoarray_to_u16(const atom_t arr[2]);  
 #define bna_int_len(bna) ( bna_is_big(bna) ? samb_twoarray_to_u16((bna) + 1) : (bna)[1] )
 
 #define bna_frac_len(bna) ( bna_is_big(bna) ? samb_twoarray_to_u16((bna) + 3) : (bna)[2] )
 
 #define bna_new_1b_10_u64(value, flags) to_digit_array(0, value, flags, TYP_NONE)
 #define bna_new_1b_256_u64(value, flags) to_digit_array(0, value, flags, TYP_ZENZ)
 
 #define bna_new_2b_10_u64(value, flags) to_digit_array(0, value, flags, TYP_BIG)
 #define bna_new_2b_256_u64(value, flags) to_digit_array(0, value, flags, TYP_BIG | TYP_ZENZ)
   
 #define bna_new_1b_10_ldbl(value) to_digit_array(value, 0, flags, TYP_NONE)
 #define bna_new_1b_256_ldbl(value) to_digit_array(value, 0, flags, TYP_ZENZ)
 
 #define bna_new_2b_10_ldbl(value) to_digit_array(value, 0, flags, TYP_BIG)
 #define bna_new_2b_256_ldbl(value) to_digit_array(value, 0, flags, TYP_BIG | TYP_ZENZ)
    
 #define sz(type, n) ( ((size_t) n) * (sizeof (type)) )
 
 #define alloc(type, size) ((type*) malloc(( (size_t) size) * sizeof (type)))
 
 #define zalloc(type, size) ((type*) calloc(( (size_t) size), sizeof (type)))
 #define macrogetval(x) #x
 #define stringify(x) macrogetval(x)
 
 #ifndef set_out_param
 #define set_out_param(name, value) do { if ( NULL != (name) ){ *name = value; } } while(0)
 #endif
 
 #ifndef string_is_sempty
 #define string_is_sempty(str, n) (NULL == str || ! strnlen_c(str, n))
 #endif
 
 #ifndef MATH_TAYLOR_ITERATIONS
 #ifdef DEBUG
 #define MATH_TAYLOR_ITERATIONS 1
 #else
 #define MATH_TAYLOR_ITERATIONS 1000
 #endif
 #endif
 
 #ifndef log_b10
 #define log_b10(a, b, c, d, e) impl_log_b10(a, b, c, d, e, MATH_TAYLOR_ITERATIONS)
 #endif
 
 #ifndef pow_b10
 #define pow_b10(a, b, c, d, e) impl_pow_b10(a, b, c, d, e, MATH_TAYLOR_ITERATIONS)
 #endif
 
 #ifndef min
 #define min(a, b) (a < b ? a : b)
 #endif
 
 #ifndef max
 #define max(a, b) (a > b ? a : b)
 #endif
   typedef enum{ BN_NONE=0,  BN_SIGN=FL_SIGN, BN_SNAN=FL_SIGN|FL_NAN, BN_SINF=FL_SIGN|FL_INF,  BN_NAN=FL_NAN, BN_INF=FL_INF  }bignum_flag_t;   float log256f(const float x); bool compare_eps(const ldbl_t a,const ldbl_t b,const ldbl_t eps); atom_t count_digits_u64(const uint64_t x); atom_t indexable_digits_u64(const uint64_t x); atom_t count_b256_digits_u64(const uint64_t x); uint16_t count_b256_digits_b10_digits(const char*const digits); atom_t get_left_nth_digit(const uint64_t x,const atom_t n); atom_t count_frac_digits(const char*const str); atom_t find_frac_beginning(const char*const str); bool raw_is_zero(const atom_t*const digits,const uint16_t len);  size_t strnlen_c(const char*const s,const size_t maxsize); char*strndup_c(const char*const s,size_t const n); char*str_reverse(const char*const str); size_t str_count(const char*const str,const char find); char*make_empty_string(void); void _say_atom_t_ptr(const atom_t*const data,const uint16_t len); 
 #ifdef DEBUG
 #ifndef say_atom_t_ptr
 #define say_atom_t_ptr(data, len) do{printf("%s ", #data); _say_atom_t_ptr(data, len);}while(0)
 #endif
 #else
 #ifndef say_atom_t_ptr
 #define say_atom_t_ptr(data, len)
 #endif
 #endif
  uint16_t array_span(const atom_t*arr,const uint16_t arr_len,const bool accept,const atom_t*const vals,const uint16_t vals_len);  bool array_contains(const atom_t*const arr,const uint16_t len,const atom_t value); atom_t*array_concat(const atom_t*const a,const uint16_t a_len,const atom_t*const b,const uint16_t b_len); atom_t*array_reverse(const atom_t*const arr,const uint16_t len); atom_t*array_copy(const atom_t*const a,const uint16_t len);  atom_t*array_trim_leading_zeroes(const atom_t*const bn); atom_t*array_trim_trailing_zeroes(const atom_t*const bn); atom_t*array_trim_trailing_zeroes_simple(const atom_t*const bn,const uint16_t len,uint16_t*const out_len); atom_t*array_trim_leading_zeroes_simple(const atom_t*const bn,const uint16_t len,uint16_t*const out_len);    bignum_t*bignum_ctor(const ldbl_t ldbl,const uint64_t u64,const atom_t flags,const bignum_t*const*const opt_vals); bignum_t*bignum_copy(const bignum_t*const bn,const bool no_recurse_optionals);   atom_t*make_array_header(const atom_t metadata,const uint16_t int_digits,const uint16_t flot_digits,const atom_t flags); atom_t*to_digit_array(const ldbl_t ldbl_in,const uint64_t u64,const atom_t value_flags,const atom_t metadata);   void samb_u16_to_twoba(const uint16_t n,atom_t*const ah,atom_t*const al); uint16_t samb_twoba_to_u16(const atom_t ah,const atom_t al);      char*b10_to_ldbl_digits(const atom_t*const digits,const uint16_t len,const uint16_t int_len); char*b10_to_u64_digits(const atom_t*const digits,const uint16_t len); uint64_t b10_to_u64(const atom_t*const digits,const uint16_t len); atom_t*ldbl_digits_to_b10(const char*const ldbl_digits,uint16_t*const len,uint16_t*const int_len,const bool little_endian); atom_t*u64_to_b10(const uint64_t value,uint16_t*const len,const bool little_endian); atom_t*u64_digits_to_b10(const char*const digits,uint16_t*const len,const bool little_endian);  uint16_t b10_to_u16(const atom_t*const,const uint16_t len);   char*b256_to_ldbl_digits(const atom_t*const digits,const uint16_t len,const uint16_t int_len); char*b256_to_u64_digits(const atom_t*const digits,const uint16_t len); uint64_t b256_to_u64(const atom_t*const digits,const uint16_t len); atom_t*ldbl_digits_to_b256(const char*const ldbl_digits,uint16_t*const len,uint16_t*const int_len,const bool little_endian); atom_t*u64_to_b256(const uint64_t value,uint16_t*const len,const bool little_endian); atom_t*u64_digits_to_b256(const char*const digits,uint16_t*const len,const bool little_endian);         atom_t*succ_b10(const atom_t*const n,const uint16_t len,const uint16_t int_len,const uint16_t precision,uint16_t*const out_len,uint16_t*const out_int_len); atom_t*pred_b10(const atom_t*const n,const uint16_t len,const uint16_t int_len,const uint16_t precision,uint16_t*const out_len,uint16_t*const out_int_len);  atom_t*impl_log_b10(const atom_t*const n,const uint16_t n_len,const uint16_t n_int_len,uint16_t*const out_len,uint16_t*const out_int_len,const uint16_t iterations);  atom_t*logn_b10(const atom_t*const base,const uint16_t base_len,const uint16_t base_int_len,const atom_t*const n,const uint16_t n_len,const uint16_t n_int_len,uint16_t*const out_len,uint16_t*const out_int_len); atom_t*add_b10(const atom_t*const a,const uint16_t a_len,const uint16_t a_int_len,const atom_t*const b,const uint16_t b_len,const uint16_t b_int_len,uint16_t*const out_len,uint16_t*const out_int_len); atom_t*sub_b10(const atom_t*const a,const uint16_t a_len,const uint16_t a_int_len,const atom_t*const b,const uint16_t b_len,const uint16_t b_int_len,uint16_t*const out_len,uint16_t*const out_int_len); atom_t*mul_b10(const atom_t*const a,const uint16_t a_len,const uint16_t a_int_len,const atom_t*const b,const uint16_t b_len,const uint16_t b_int_len,uint16_t*const out_len,uint16_t*const out_int_len); atom_t*div_b10(const atom_t*const a,const uint16_t a_len,const uint16_t a_int_len,const atom_t*const b,const uint16_t b_len,const uint16_t b_int_len,uint16_t*const out_len,uint16_t*const out_int_len); atom_t*impl_pow_b10(const atom_t*const a,const uint16_t a_len,const uint16_t a_int_len,const atom_t*const b,const uint16_t b_len,const uint16_t b_int_len,uint16_t*const out_len,uint16_t*const out_int_len);  atom_t*times2_b10(const atom_t*const n,const uint16_t len,const uint16_t int_len,uint16_t*const out_len,uint16_t*const out_int_len); atom_t*sq_b10(const atom_t*const n,const uint16_t len,const uint16_t int_len,uint16_t*const out_len,uint16_t*const out_int_len); atom_t*recip_b10(const atom_t*const n,const uint16_t len,const uint16_t int_len,uint16_t*const out_len,uint16_t*const out_int_len); atom_t*floor_b10(const atom_t*const n,const uint16_t len,const uint16_t int_len,uint16_t*const out_len,uint16_t*const out_int_len); atom_t*ceil_b10(const atom_t*const n,const uint16_t len,const uint16_t int_len,uint16_t*const out_len,uint16_t*const out_int_len);  atom_t cmp_b10(const atom_t*const a,const uint16_t a_len,const uint16_t a_int_len,const atom_t*const b,const uint16_t b_len,const uint16_t b_int_len);  atom_t*factorial_b10(const atom_t*const n,const uint16_t len,const uint16_t int_len,uint16_t*const out_len,uint16_t*out_int_len); 
 #endif 
 
#endif
