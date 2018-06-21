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

#ifndef YACBNL
 #define YACBNL
                                  
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
 
 #ifndef LOG_ITERATIONS
 #define LOG_ITERATIONS 1
 #endif
 
 #ifndef log_b10
 #define log_b10(a, b, c, d, e) impl_log_b10(a, b, c, d, e, LOG_ITERATIONS)
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
  uint16_t array_span(const atom_t*arr,const uint16_t arr_len,const bool accept,const atom_t*const vals,const uint16_t vals_len);  bool array_contains(const atom_t*const arr,const uint16_t len,const atom_t value); atom_t*array_concat(const atom_t*const a,const uint16_t a_len,const atom_t*const b,const uint16_t b_len); atom_t*array_reverse(const atom_t*const arr,const uint16_t len); atom_t*array_copy(const atom_t*const a,const uint16_t len);  atom_t*array_trim_leading_zeroes(const atom_t*const bn); atom_t*array_trim_trailing_zeroes(const atom_t*const bn); atom_t*array_trim_trailing_zeroes_simple(const atom_t*const bn,const uint16_t len,uint16_t*const out_len); atom_t*array_trim_leading_zeroes_simple(const atom_t*const bn,const uint16_t len,uint16_t*const out_len);    bignum_t*bignum_ctor(const ldbl_t ldbl,const uint64_t u64,const atom_t flags,const bignum_t*const*const opt_vals); bignum_t*bignum_copy(const bignum_t*const bn,const bool no_recurse_optionals);   atom_t*make_array_header(const atom_t metadata,const uint16_t int_digits,const uint16_t flot_digits,const atom_t flags); atom_t*to_digit_array(const ldbl_t ldbl_in,const uint64_t u64,const atom_t value_flags,const atom_t metadata);   void samb_u16_to_twoba(const uint16_t n,atom_t*const ah,atom_t*const al); uint16_t samb_twoba_to_u16(const atom_t ah,const atom_t al);      char*b10_to_ldbl_digits(const atom_t*const digits,const uint16_t len,const uint16_t int_len); char*b10_to_u64_digits(const atom_t*const digits,const uint16_t len); uint64_t b10_to_u64(const atom_t*const digits,const uint16_t len); atom_t*ldbl_digits_to_b10(const char*const ldbl_digits,uint16_t*const len,uint16_t*const int_len,const bool little_endian); atom_t*u64_to_b10(const uint64_t value,uint16_t*const len,const bool little_endian); atom_t*u64_digits_to_b10(const char*const digits,uint16_t*const len,const bool little_endian);  uint16_t b10_to_u16(const atom_t*const,const uint16_t len);   char*b256_to_ldbl_digits(const atom_t*const digits,const uint16_t len,const uint16_t int_len); char*b256_to_u64_digits(const atom_t*const digits,const uint16_t len); uint64_t b256_to_u64(const atom_t*const digits,const uint16_t len); atom_t*ldbl_digits_to_b256(const char*const ldbl_digits,uint16_t*const len,uint16_t*const int_len,const bool little_endian); atom_t*u64_to_b256(const uint64_t value,uint16_t*const len,const bool little_endian); atom_t*u64_digits_to_b256(const char*const digits,uint16_t*const len,const bool little_endian);         atom_t*succ_b10(const atom_t*const n,const uint16_t len,const uint16_t int_len,const uint16_t precision,uint16_t*const out_len,uint16_t*const out_int_len); atom_t*pred_b10(const atom_t*const n,const uint16_t len,const uint16_t int_len,const uint16_t precision,uint16_t*const out_len,uint16_t*const out_int_len);  atom_t*impl_log_b10(const atom_t*const n,const uint16_t n_len,const uint16_t n_int_len,uint16_t*const out_len,uint16_t*const out_int_len,const uint16_t iterations);  atom_t*logn_b10(const atom_t*const base,const uint16_t base_len,const uint16_t base_int_len,const atom_t*const n,const uint16_t n_len,const uint16_t n_int_len,uint16_t*const out_len,uint16_t*const out_int_len); atom_t*add_b10(const atom_t*const a,const uint16_t a_len,const uint16_t a_int_len,const atom_t*const b,const uint16_t b_len,const uint16_t b_int_len,uint16_t*const out_len,uint16_t*const out_int_len); atom_t*sub_b10(const atom_t*const a,const uint16_t a_len,const uint16_t a_int_len,const atom_t*const b,const uint16_t b_len,const uint16_t b_int_len,uint16_t*const out_len,uint16_t*const out_int_len); atom_t*mul_b10(const atom_t*const a,const uint16_t a_len,const uint16_t a_int_len,const atom_t*const b,const uint16_t b_len,const uint16_t b_int_len,uint16_t*const out_len,uint16_t*const out_int_len); atom_t*div_b10(const atom_t*const a,const uint16_t a_len,const uint16_t a_int_len,const atom_t*const b,const uint16_t b_len,const uint16_t b_int_len,uint16_t*const out_len,uint16_t*const out_int_len); atom_t*pow_b10(const atom_t*const a,const uint16_t a_len,const uint16_t a_int_len,const atom_t*const b,const uint16_t b_len,const uint16_t b_int_len,uint16_t*const out_len,uint16_t*const out_int_len);  atom_t*times2_b10(const atom_t*const n,const uint16_t len,const uint16_t int_len,uint16_t*const out_len,uint16_t*const out_int_len); atom_t*sq_b10(const atom_t*const n,const uint16_t len,const uint16_t int_len,uint16_t*const out_len,uint16_t*const out_int_len); atom_t*recip_b10(const atom_t*const n,const uint16_t len,const uint16_t int_len,uint16_t*const out_len,uint16_t*const out_int_len); atom_t*floor_b10(const atom_t*const n,const uint16_t len,const uint16_t int_len,uint16_t*const out_len,uint16_t*const out_int_len); 
 #endif 
 
 #ifndef MATH_PRIMITIVE_BASE10
 #define MATH_PRIMITIVE_BASE10
  static atom_t*impl_pred_b10_int(const atom_t*const n,const uint16_t len,uint16_t*const out_len){  if(0!=n[len-1]){ set_out_param(out_len,len);  atom_t*const result=(atom_t*)memcpy(alloc(atom_t,len),n,(uint16_t)(len-1));  result[len-1]=(atom_t)((atom_t)n[len-1]-1); return result; }else{ atom_t*const z=zalloc(atom_t,1); const uint16_t count_leading_digits=array_span(n,len,false,z,1); free(z); atom_t*const pred_leading_digits=(atom_t*)memcpy(alloc(atom_t,count_leading_digits),n,count_leading_digits); pred_leading_digits[count_leading_digits-1]=(atom_t)(pred_leading_digits[count_leading_digits-1]-1); const uint16_t count_trailing_zeroes=(uint16_t)(len-count_leading_digits); atom_t*const nines=(atom_t*)memset(alloc(atom_t,count_trailing_zeroes),9,count_trailing_zeroes); atom_t*const all_digits=array_concat(pred_leading_digits,count_leading_digits,nines,count_trailing_zeroes); free(nines),free(pred_leading_digits); atom_t*const final=array_trim_leading_zeroes_simple(all_digits,len,out_len); free(all_digits); return final; } } static atom_t*impl_pred_b10_flot(const atom_t*const n,const uint16_t len,const uint16_t int_len,const uint16_t precision,uint16_t*const out_len,uint16_t*const out_int_len){   const uint16_t focus_digit=(uint16_t)((int_len+precision)-1); if(0!=n[focus_digit]){   set_out_param(out_len,len); set_out_param(out_int_len,int_len);  atom_t*const result=(atom_t*)memcpy(alloc(atom_t,len),n,len);  result[focus_digit]=(atom_t)(n[focus_digit]-1); return result; }else{    puts("UNIMPLEMENTED"); return NULL; } }                 atom_t*pred_b10(const atom_t*const n,const uint16_t len,const uint16_t int_len,const uint16_t precision,uint16_t*const out_len,uint16_t*const out_int_len){ if((len==int_len&&raw_is_zero(n,int_len))||raw_is_zero(n,len)){ set_out_param(out_len,0); return zalloc(atom_t,1); } if(len==int_len&&0==precision){ return impl_pred_b10_int(n,len,out_len); }else{ return impl_pred_b10_flot(n,len,int_len,precision,out_len,out_int_len); } } static atom_t*impl_succ_b10_int(const atom_t*const n,const uint16_t len,uint16_t*const out_len){ if(9!=n[len-1]){  set_out_param(out_len,len);  atom_t*const result=(atom_t*)memcpy(alloc(atom_t,len),n,len);  result[len-1]=(atom_t)(n[len-1]+1); }else{ atom_t*const reversed=array_reverse(n,len); atom_t*const nine=memset(alloc(atom_t,1),9,1);  const uint16_t count_nines=array_span(reversed,len,true,nine,1); free(nine),free(reversed); const uint16_t other_digits_pre_len=(uint16_t)(len-count_nines); const uint16_t other_digits_len=0==other_digits_pre_len ? 1 : other_digits_pre_len; atom_t*const zeroes=zalloc(atom_t,count_nines); atom_t*other_digits; if(0==other_digits_pre_len){  atom_t*const one_zero=zalloc(atom_t,1); other_digits=memcpy(alloc(atom_t,other_digits_len),one_zero,other_digits_len); free(one_zero); }else{  other_digits=memcpy(alloc(atom_t,other_digits_len),reversed,other_digits_len); }  other_digits[other_digits_len-1]=(atom_t)(other_digits[other_digits_len-1]+1); atom_t*const result=array_concat(other_digits,other_digits_len,zeroes,count_nines); free(zeroes),free(other_digits); return result; } puts("UNIMPLEMENTED"); (void)n; (void)len; (void)out_len; return NULL; } static atom_t*impl_succ_b10_flot(const atom_t*const n,const uint16_t len,const uint16_t int_len,const uint16_t precision,uint16_t*const out_len,uint16_t*const out_int_len){ (void)n; (void)len; (void)precision; (void)int_len; (void)out_len; (void)out_int_len; return NULL; }    atom_t*succ_b10(const atom_t*const n,const uint16_t len,const uint16_t int_len,const uint16_t precision,uint16_t*const out_len,uint16_t*const out_int_len){ if(raw_is_zero(n,len)){ set_out_param(out_len,1); atom_t*const res=alloc(atom_t,1); res[0]=1; return res; } if(len==int_len&&0==precision){ return impl_succ_b10_int(n,len,out_len); }else{ return impl_succ_b10_flot(n,len,int_len,precision,out_len,out_int_len); } } atom_t*add_b10(const atom_t*const a,const uint16_t a_len,const uint16_t a_int_len,const atom_t*const b,const uint16_t b_len,const uint16_t b_int_len,uint16_t*const out_len,uint16_t*const out_int_len){ puts("UNIMPLEMENTED"); (void)a; (void)a_len; (void)a_int_len; (void)b; (void)b_len; (void)b_int_len; (void)out_len; (void)out_int_len; return NULL; } atom_t*sub_b10(const atom_t*const a,const uint16_t a_len,const uint16_t a_int_len,const atom_t*const b,const uint16_t b_len,const uint16_t b_int_len,uint16_t*const out_len,uint16_t*const out_int_len){ puts("UNIMPLEMENTED"); (void)a; (void)a_len; (void)a_int_len; (void)b; (void)b_len; (void)b_int_len; (void)out_len; (void)out_int_len; return NULL; } atom_t*mul_b10(const atom_t*const a,const uint16_t a_len,const uint16_t a_int_len,const atom_t*const b,const uint16_t b_len,const uint16_t b_int_len,uint16_t*const out_len,uint16_t*const out_int_len){ puts("UNIMPLEMENTED"); (void)a; (void)a_len; (void)a_int_len; (void)b; (void)b_len; (void)b_int_len; (void)out_len; (void)out_int_len; return NULL; } atom_t*div_b10(const atom_t*const a,const uint16_t a_len,const uint16_t a_int_len,const atom_t*const b,const uint16_t b_len,const uint16_t b_int_len,uint16_t*const out_len,uint16_t*const out_int_len){ puts("UNIMPLEMENTED"); (void)a; (void)a_len; (void)a_int_len; (void)b; (void)b_len; (void)b_int_len; (void)out_len; (void)out_int_len; return NULL; } atom_t*pow_b10(const atom_t*const a,const uint16_t a_len,const uint16_t a_int_len,const atom_t*const b,const uint16_t b_len,const uint16_t b_int_len,uint16_t*const out_len,uint16_t*const out_int_len){ puts("UNIMPLEMENTED"); (void)a; (void)a_len; (void)a_int_len; (void)b; (void)b_len; (void)b_int_len; (void)out_len; (void)out_int_len; return NULL; } atom_t*times2_b10(const atom_t*const n,const uint16_t len,const uint16_t int_len,uint16_t*const out_len,uint16_t*const out_int_len){ puts("UNIMPLEMENTED"); (void)n; (void)len; (void)int_len; (void)out_len; (void)out_int_len; return NULL; } atom_t*sq_b10(const atom_t*const n,const uint16_t len,const uint16_t int_len,uint16_t*const out_len,uint16_t*const out_int_len){ puts("UNIMPLEMENTED"); (void)n; (void)len; (void)int_len; (void)out_len; (void)out_int_len; return NULL; } atom_t*recip_b10(const atom_t*const n,const uint16_t len,const uint16_t int_len,uint16_t*const out_len,uint16_t*const out_int_len){ puts("UNIMPLEMENTED"); (void)n; (void)len; (void)int_len; (void)out_len; (void)out_int_len; return NULL; } atom_t*floor_b10(const atom_t*const n,const uint16_t len,const uint16_t int_len,uint16_t*const out_len,uint16_t*const out_int_len){ puts("UNIMPLEMENTED"); (void)n; (void)len; (void)int_len; (void)out_len; (void)out_int_len; return NULL; }  atom_t*impl_log_b10(const atom_t*const n,const uint16_t n_len,const uint16_t n_int_len,uint16_t*const out_len,uint16_t*const out_int_len,const uint16_t iterations){       uint16_t total_len=0,total_int_len=0, n_succ_len=0,n_succ_int_len=0, n_succ_sq_len=0,n_succ_sq_int_len=0, n_pred_len=0,n_pred_int_len=0, n_pred_sq_len=0,n_pred_sq_int_len=0, run_mul_len=0,run_mul_int_len=0, power_len=0,power_int_len=0, y_len=0,y_int_len=0;     atom_t*const zero=zalloc(atom_t,1), *total=array_copy(zero,1), *power=succ_b10(zero,1,1,0,&power_len,&power_int_len), *y=array_copy(zero,1), *const n_succ=succ_b10(n,n_len,n_int_len,0,&n_succ_len,&n_succ_int_len), *const n_succ_sq=sq_b10(n_succ,n_succ_len,n_succ_int_len,&n_succ_sq_len,&n_succ_sq_int_len), *const n_pred=pred_b10(n,n_len,n_int_len,0,&n_pred_len,&n_pred_int_len), *const n_pred_sq=sq_b10(n_pred,n_pred_len,n_pred_int_len,&n_pred_sq_len,&n_pred_sq_int_len), *run_mul=div_b10(n_succ,n_succ_len,n_succ_int_len,n_pred,n_pred_len,n_pred_int_len,&run_mul_len,&run_mul_int_len); for(uint16_t i=0;i<iterations;i++){ free(y);   const uint16_t temp_run_mul_len=run_mul_len,temp_run_mul_int_len=run_mul_int_len; atom_t*const temp_run_mul=array_copy(run_mul,run_mul_len); free(run_mul);   uint16_t mul_step_len=0,mul_step_int_len=0; atom_t*const mul_step=div_b10(n_pred_sq,n_pred_sq_len,n_pred_sq_int_len,n_succ_sq,n_succ_sq_len,n_succ_sq_len,&mul_step_len,&mul_step_int_len); run_mul=mul_b10(temp_run_mul,temp_run_mul_len,temp_run_mul_int_len,mul_step,mul_step_len,mul_step_int_len,&run_mul_len,&run_mul_int_len); free(mul_step); free(temp_run_mul);  uint16_t recip_power_len=0,recip_power_int_len=0; atom_t*const recip_power=recip_b10(power,power_len,power_int_len,&recip_power_len,&recip_power_int_len); y=mul_b10(recip_power,recip_power_len,recip_power_int_len,run_mul,run_mul_len,run_mul_int_len,&y_len,&y_int_len); free(recip_power);  total=add_b10(total,total_len,total_int_len,y,y_len,y_int_len,&total_len,&total_int_len);   power=impl_succ_b10_int(power,power_len,&power_len); power_int_len=power_len; power=impl_succ_b10_int(power,power_len,&power_len); power_int_len=power_len; } free(zero); free(total); free(power); free(y); free(n_succ); free(n_succ_sq); free(n_pred); free(n_pred_sq); free(run_mul); atom_t*const final=times2_b10(total,total_len,total_int_len,out_len,out_int_len); free(total); return final; } atom_t*logn_b10(const atom_t*const base,const uint16_t base_len,const uint16_t base_int_len,const atom_t*const n,const uint16_t n_len,const uint16_t n_int_len,uint16_t*const out_len,uint16_t*const out_int_len){ uint16_t log_base_int_len=0,log_base_len=0; atom_t*const log_base=log_b10(base,base_len,base_int_len,&log_base_len,&log_base_int_len); uint16_t log_n_int_len=0,log_n_len=0; atom_t*const log_n=log_b10(n,n_len,n_int_len,&log_n_len,&log_n_int_len); atom_t*const result=div_b10(log_n,log_n_len,log_n_int_len,log_base,log_base_len,log_base_int_len,out_len,out_int_len); free(log_base); free(log_n); return result; }
 #endif 
  
 #ifndef BASE10_H
 #define BASE10_H
          char*b10_to_ldbl_digits(const atom_t*const digits,const uint16_t len,const uint16_t int_len){ if(NULL==digits||!len||int_len>len){return make_empty_string();} char*const int_part=alloc(char,int_len+1); for(size_t i=0;i<int_len;i++){ int_part[i]=(char)(digits[i]+CHAR_DIGIT_DIFF); } int_part[int_len]='\0'; const size_t flot_len=(size_t)(len-int_len); char*const flot_part=alloc(char,flot_len+1); for(size_t i=0;i<flot_len;i++){ flot_part[i]=(char)((char)digits[int_len+i]+CHAR_DIGIT_DIFF); } flot_part[flot_len]='\0'; const bool dot=len!=int_len; char*const final_concat=alloc(char,int_len+dot+flot_len); snprintf(final_concat,MAX_STR_LDBL_DIGITS,"%s%s%s",int_part,(dot ? DECIMAL_SEPARATOR_STR : ""),flot_part); free(int_part),free(flot_part); return final_concat; }      char*b10_to_u64_digits(const atom_t*const digits,const uint16_t len){ char*const u64_str=alloc(char,len+1); for(uint16_t i=0;i<len;i++){ u64_str[i]=(char)(digits[i]+CHAR_DIGIT_DIFF); } u64_str[len]='\0'; return u64_str; }       uint64_t b10_to_u64(const atom_t*const digits,const uint16_t len){ char*const u64_str=b10_to_u64_digits(digits,len); const uint64_t final=strtoull(u64_str,NULL,DEC_BASE); free(u64_str); return final; } uint16_t b10_to_u16(const atom_t*const n,const uint16_t len){ (void)n; (void)len; return 0; }      atom_t*ldbl_digits_to_b10(const char*const ldbl_digits,uint16_t*const len,uint16_t*const int_len,const bool little_endian){ if((0==strnlen_c(ldbl_digits,MAX_U64_DIGITS+2))||NULL==ldbl_digits||NULL==len||NULL==int_len){ set_out_param(len,1); set_out_param(int_len,1); return zalloc(atom_t,1); }  const uint16_t pre_dec=(uint16_t)strcspn(ldbl_digits,DECIMAL_SEPARATOR_STR), digits_len=(uint16_t)strnlen_c(ldbl_digits,MAX_STR_LDBL_DIGITS); char*const int_part=strndup_c(ldbl_digits,pre_dec), *const flot_part=strndup_c(ldbl_digits+pre_dec+1,MAX_STR_LDBL_DIGITS); uint16_t i=0; for(;int_part[i];i++){ int_part[i]=(char)(int_part[i]-CHAR_DIGIT_DIFF); } const uint16_t int_part_len=i; for(i=0;flot_part[i];i++){ flot_part[i]=(char)(flot_part[i]-CHAR_DIGIT_DIFF); } const uint16_t flot_part_len=i; set_out_param(len,(uint16_t)(int_part_len+flot_part_len)); set_out_param(int_len,int_part_len); atom_t*const res=alloc(atom_t,digits_len); memcpy(res,int_part,int_part_len); memcpy(res+int_part_len+1,flot_part,flot_part_len); free(int_part),free(flot_part); if(little_endian){ atom_t*const res_rev=array_reverse(res,(uint16_t)(int_part_len+flot_part_len)); free(res); return res_rev; } return res; } atom_t*u64_to_b10(const uint64_t value,uint16_t*const len,const bool little_endian){ char*const str_digits=alloc(char,MAX_U64_DIGITS+1); snprintf(str_digits,MAX_U64_DIGITS,"%" PRIu64,value); atom_t*const bytes=u64_digits_to_b10(str_digits,len,little_endian); free(str_digits); return bytes; }  atom_t*u64_digits_to_b10(const char*const digits,uint16_t*const len,const bool little_endian){ const size_t digits_len=strnlen_c(digits,MAX_STR_LDBL_DIGITS); if(NULL==digits||0==digits_len||NULL==len){ return NULL; } set_out_param(len,(uint16_t)digits_len); atom_t*const as_b10=alloc(atom_t,digits_len); for(uint16_t i=0;i<digits_len;i++){ as_b10[i]=(atom_t)((char)digits[i]-CHAR_DIGIT_DIFF); } if(little_endian){ atom_t*const rev_digits=array_reverse(as_b10,(uint16_t)digits_len); free(as_b10); return rev_digits; } return as_b10; }
 #endif
  
 #ifndef BIGNUM_H
 #define BIGNUM_H
                                                                    
 #endif 
  
 #ifndef BNA_H
 #define BNA_H
 static atom_t*impl_to_digit_array_ldbl(const ldbl_t ldbl,const atom_t metadata,const atom_t flags); static atom_t*impl_to_digit_array_u64(const uint64_t u64,const atom_t metadata,const atom_t flags);  atom_t*make_array_header(const atom_t metadata,const uint16_t int_digits,const uint16_t flot_digits,const atom_t flags){ const atom_t hdrlen=meta_header_offset(metadata); atom_t*const header=zalloc(atom_t,hdrlen);  header[0]=metadata;  header[hdrlen-1]=flags; if(meta_is_big(metadata)){ atom_t lens[]={0,0,0,0};  samb_u16_to_twoba(int_digits,lens+0,lens+1); samb_u16_to_twoba(flot_digits,lens+2,lens+3);  memcpy(header+1,&lens,sz(atom_t,4)); }else{ header[1]=(atom_t)int_digits; header[2]=(atom_t)flot_digits; } return header; }         atom_t*to_digit_array(const ldbl_t ldbl_in,const uint64_t u64,const atom_t value_flags,const atom_t metadata){ const atom_t flags=ldbl_in<0 ? value_flags|FL_SIGN : value_flags; const ldbl_t ldbl=ldbl_in<0 ? fabsl(ldbl_in): ldbl_in;  if(!compare_eps(ldbl,0.f,COMPARE_EPS)){ return impl_to_digit_array_ldbl(ldbl,metadata,flags);  }else if(0!=u64){ return impl_to_digit_array_u64(u64,metadata,flags); }else{ return make_array_header(metadata,0,0,flags); } return NULL; }         static atom_t*impl_to_digit_array_ldbl(const ldbl_t ldbl,const atom_t metadata,const atom_t flags){  const bool is_base256=meta_is_base256(metadata), is_big=meta_is_big(metadata);  const atom_t hdrlen=meta_header_offset(metadata);  const uint32_t sigfigs=is_big ? MAX_EXPORT_SIGFIGS_BIG : MAX_EXPORT_SIGFIGS;  char*const fullstr=alloc(char,sigfigs+3); snprintf(fullstr,sigfigs+2,"%LG",ldbl); const atom_t 
 #ifdef PREFER_CHAR_CONV
  nint_digits=(atom_t)strcspn(fullstr,DECIMAL_SEPARATOR_STR),
 #else 
  nint_digits=count_digits_u64((uint64_t)floorl(ldbl)),
 #endif 
 nflot_digits=count_frac_digits(fullstr);    atom_t*bn_tlated=alloc(atom_t,nint_digits+nflot_digits+hdrlen), *const init=make_array_header(metadata,nint_digits,nflot_digits,flags);  memcpy(bn_tlated,init,sz(atom_t,hdrlen)); free(init); if(is_base256){  uint16_t len,int_len;  atom_t*as_digits=ldbl_digits_to_b256(fullstr,&len,&int_len,true); free(fullstr);  memcpy(bn_tlated+hdrlen,as_digits,len); }else{ 
 #ifdef PREFER_CHAR_CONV
  char*const integ_str=strndup_c(fullstr,nint_digits); for(atom_t i=0;i<nint_digits;i++){ bn_tlated[i+hdrlen]=(atom_t)((unsigned)integ_str[i]-'0'); } free(integ_str);
 #else 
  for(atom_t i=0;i<nint_digits;i++){ bn_tlated[i+hdrlen]=get_left_nth_digit((uint64_t)floorl(ldbl),i); }
 #endif 
       const char*const frac_str=fullstr+find_frac_beginning(fullstr); for(size_t i=0;i<nflot_digits;i++){ bn_tlated[i+hdrlen+nint_digits]=(atom_t)((unsigned)frac_str[i]-'0'); } } free(fullstr); return bn_tlated; }        static atom_t*impl_to_digit_array_u64(const uint64_t u64,const atom_t metadata,const atom_t flags){    const bool using_base256=meta_is_base256(metadata);   const atom_t ndigits=using_base256 ? count_b256_digits_u64(u64): count_digits_u64(u64), hdrlen=meta_header_offset(metadata);  atom_t*bn_tlated=alloc(atom_t,ndigits+hdrlen), *const init=make_array_header(metadata,ndigits,0,flags);  memcpy(bn_tlated,init,sz(atom_t,hdrlen)); free(init);  if(using_base256){  uint16_t len=0;  atom_t*as_digits=u64_to_b256(u64,&len,true);  memcpy(bn_tlated+hdrlen,as_digits,len); free(as_digits); }else{
 #ifdef PREFER_CHAR_CONV
  char*const str=alloc(char,ndigits+2); snprintf(str,MAX_U64_DIGITS+1,"%" PRIu64 "",u64); for(atom_t i=0;i<ndigits;i++){ bn_tlated[i+hdrlen]=(atom_t)((unsigned)str[i]-'0'); } free(str); 
 #else 
 for(atom_t i=0;i<ndigits;i++){  bn_tlated[i+hdrlen]=get_left_nth_digit(u64,i); }
 #endif 
 } return bn_tlated; }
 #endif 
  
 #ifndef BASE256_H
 #define BASE256_H
           atom_t*u64_digits_to_b256(const char*const u64_str,uint16_t*const len,const bool little_endian){ if(string_is_sempty(u64_str,MAX_STR_LDBL_DIGITS)||NULL==len){ set_out_param(len,1); return zalloc(atom_t,1); } atom_t*const result=zalloc(atom_t,count_b256_digits_b10_digits(u64_str)); atom_t*const as_b10=u64_digits_to_b10(u64_str,len,false); (void)little_endian; (void)as_b10; puts("UNIMPLEMENTED");           return result; }    static atom_t*str_digits_to_b256(const char*const digits,uint16_t*const len,const bool little_endian){ puts("UNIMPLEMENTED"); (void)digits; (void)len; (void)little_endian; return NULL; }                         atom_t*ldbl_digits_to_b256(const char*const ldbl_digits,uint16_t*const len,uint16_t*const int_len,const bool little_endian){ if(string_is_sempty(ldbl_digits,MAX_STR_LDBL_DIGITS)||NULL==len||NULL==int_len){ set_out_param(len,1); set_out_param(len,1); return zalloc(atom_t,1); }  const uint16_t pre_dec=(uint16_t)strcspn(ldbl_digits,DECIMAL_SEPARATOR_STR);  char*const int_part=strndup_c(ldbl_digits,pre_dec),  *const flot_part=str_reverse(ldbl_digits+pre_dec+1); uint16_t lhs_len=0,rhs_len=0; atom_t*const lhs_b256=str_digits_to_b256(int_part,&lhs_len,true),  *const rhs_b256=str_digits_to_b256(flot_part,&rhs_len,false); set_out_param(len,(uint16_t)(lhs_len+rhs_len)); set_out_param(int_len,lhs_len); atom_t*const final_concat=array_concat(lhs_b256,lhs_len,rhs_b256,rhs_len); free(lhs_b256),free(rhs_b256); if(little_endian){ atom_t*const reversed=array_reverse(final_concat,*len); free(final_concat); return reversed; }else{ return final_concat; } }                 uint64_t b256_to_u64(const atom_t*const digits,const uint16_t len){ if(!digits||!len){ return 0; } errno=0; uint64_t result=0; if(len>MAX_U64_DIGITS){goto range_err;} for(int16_t i=(int16_t)(len-1);i>-1;i--){ result+=digits[i]*((uint64_t)powl(ZENZ_BASE,i)); if(ERANGE==errno){goto range_err;} } return result; range_err: errno=ERANGE; return 0; } char*b256_to_u64_digits(const atom_t*const digits,const uint16_t len){ (void)digits; (void)len; return NULL; }          char*b256_to_ldbl_digits(const atom_t*const digits,const uint16_t len,const uint16_t int_len){   if(NULL==digits||!len||int_len>len){ return make_empty_string(); }   const uint16_t int_len_orig=int_len;  const uint16_t flot_len_orig=(uint16_t)(len-int_len_orig);  atom_t*const int_b256=(atom_t*)memcpy(alloc(atom_t,int_len_orig),digits,sz(atom_t,int_len_orig)),  *const flot_b256=array_reverse(digits+int_len_orig,flot_len_orig);   char*const int_b10=b256_to_u64_digits(int_b256,int_len_orig),  *const flot_b10=b256_to_u64_digits(flot_b256,flot_len_orig),  *const flot_b10_be=str_reverse(flot_b10);  free(int_b10),free(flot_b256),free(flot_b10);  const uint16_t int_len_b10=(uint16_t)strnlen_c(int_b10,MAX_STR_LDBL_DIGITS), flot_len_b10=(uint16_t)strnlen_c(flot_b10_be,MAX_STR_LDBL_DIGITS);  char*const out_str=alloc(char,2+int_len_b10+flot_len_b10); snprintf(out_str,1U+int_len_b10+flot_len_b10,"%s.%s",int_b10,flot_b10_be);  free(int_b10),free(flot_b10_be);   return out_str; } atom_t*u64_to_b256(const uint64_t value,uint16_t*const len,const bool little_endian){ char*const val_str=alloc(char,count_digits_u64(value)); snprintf(val_str,MAX_U64_DIGITS,"%" PRIu64 "",value); atom_t*const result=u64_digits_to_b256(val_str,len,little_endian); free(val_str); return result; }
 #endif 
  
 #ifndef ADDR_INTERP_H
 #define ADDR_INTERP_H
        void samb_u16_to_twoba(const uint16_t n,atom_t*const ah,atom_t*const al){ *ah=(atom_t)(n>>(atom_t)8); *al=(atom_t)(n&(atom_t)0xFF); }      uint16_t samb_twoba_to_u16(const atom_t ah,const atom_t al){  return(uint16_t)((ah<<8)|(atom_t)al); }      uint16_t samb_twoarray_to_u16(const atom_t arr[2]){ return samb_twoba_to_u16(arr[0],arr[1]); }
 #endif 
  
 #ifndef MISC_UTIL_H
 #define MISC_UTIL_H
  bool compare_eps(const ldbl_t a,const ldbl_t b,const ldbl_t eps){ return fabsl(a-b)<eps; }      atom_t count_digits_u64(const uint64_t x){ return(atom_t)floor(log10f((float)x)+1.f); }    bool raw_is_zero(const atom_t*const digits,const uint16_t len){ uint16_t sum=0; for(uint16_t i=0;i<len;i++){ sum=(uint16_t)((uint16_t)digits[i]+sum); } return 0==sum; }      atom_t find_frac_beginning(const char*const str){  const atom_t pre_len=(atom_t)strcspn(str,DECIMAL_SEPARATOR_STR), len=(atom_t)strnlen_c(str,MAX_PRIMITIVE_LDBL_DIGITS), diff=(atom_t)(len-pre_len);  if(1==diff){ return pre_len; }else if(diff){ return(atom_t)(1+pre_len); }else{ return len; } }       atom_t count_frac_digits(const char*const str){ const atom_t begin=find_frac_beginning(str); if(1==strnlen_c(str,MAX_PRIMITIVE_LDBL_DIGITS)-begin){return 1;} return(atom_t)strspn(str+begin,"0123456789"); }       atom_t indexable_digits_u64(const uint64_t x){ return(atom_t)floor(log10f((float)x)); }          atom_t get_left_nth_digit(const uint64_t x,const atom_t n){
 #ifdef PREFER_CHAR_CONV
 char*const str=alloc(char,MAX_U64_DIGITS+2); snprintf(str,MAX_U64_DIGITS+1,"%" PRIu64 "",x); const char d=str[n]; free(str); return(atom_t)((unsigned)d-'0');
 #else 
 const ldbl_t tpow=pow(DEC_BASE,indexable_digits_u64(x)-n), ldivr=((ldbl_t)x)/tpow;  return(atom_t)(((uint64_t)ldivr)%DEC_BASE);
 #endif 
 }       char*str_reverse(const char*const str){ if(NULL==str){return NULL;} size_t len=strnlen_c(str,MAX_EXPORT_SIGFIGS); char*newp=NULL; if(len<2){ newp=strndup_c(str,len); }else{ newp=alloc(char,len+1); size_t i,j; for(i=0,j=len-1;i<len;i++,j--){ newp[i]=str[j];  } newp[i]='\0'; } return newp; }     float log256f(const float x){ return logf(x)/logf(ZENZ_BASE); }     atom_t count_b256_digits_u64(const uint64_t x){ return(atom_t)floorf(log256f((float)x)+1.f); }     uint16_t count_b256_digits_b10_digits(const char*const digits){ puts("UNIMPLEMENTED"); uint16_t len_initial=0; atom_t*const as_atoms=u64_digits_to_b10(digits,&len_initial,false); static const atom_t b10_256[3]={2,5,6};  uint16_t log_len=0,log_int_len=0; atom_t*const log256=logn_b10(b10_256,3,3,as_atoms,len_initial,len_initial,&log_len,&log_int_len); free(as_atoms);  uint16_t len_add1=0,int_len_add1=0; atom_t*const add1=succ_b10(log256,len_initial,len_initial,0,&len_add1,&int_len_add1); free(log256);  uint16_t floored_len=0,floored_int_len=0; atom_t*const floored=floor_b10(add1,len_add1,int_len_add1,&floored_len,&floored_int_len); free(add1);  const uint16_t final=b10_to_u16(floored,len_add1); free(floored); return final; }      atom_t*array_reverse(const atom_t*const arr,const uint16_t len){ atom_t*result=alloc(atom_t,len); if(len){ for(uint16_t i=0;i<len;i++){ result[i]=arr[(len-1)-i ]; } } return result; }      atom_t*array_concat(const atom_t*const a,const uint16_t a_len,const atom_t*const b,const uint16_t b_len){ if(!(a_len+b_len)||NULL==a||NULL==b){ return alloc(atom_t,0); }  else if(a_len==(a_len+b_len)){ return(atom_t*)memcpy(alloc(atom_t,a_len),a,a_len); }else if(b_len==a_len+b_len){ return(atom_t*)memcpy(alloc(atom_t,b_len),b,b_len); }  atom_t*const res=(atom_t*)memcpy(alloc(atom_t,a_len+b_len),a,a_len);  memcpy(res+a_len,b,b_len);  return(atom_t*)res; } bool array_contains(const atom_t*const arr,const uint16_t len,const atom_t value){ if(len){ for(size_t i=0;i<len;i++){ if(value==arr[i]){return true;} } } return false; } atom_t*array_copy(const atom_t*const a,const uint16_t len){ return(atom_t*)memcpy(alloc(atom_t,len),a,len); }     char*make_empty_string(void){ return zalloc(char,1); }        uint16_t array_span(const atom_t*arr,const uint16_t arr_len,const bool accept,const atom_t*const vals,const uint16_t vals_len){ uint16_t i=0; for(;i<arr_len;i++){ if(accept==!array_contains(vals,vals_len,arr[i])){ break; } } return i; }      atom_t*array_trim_trailing_zeroes(const atom_t*const bn){ const atom_t hdrlen=bna_header_offset(bn); const bool is_big=bna_is_big(bn); const uint16_t  int_len=bna_int_len(bn), frc_len=bna_frac_len(bn);   puts("UNIMPLEMENTED"); (void)hdrlen; (void)is_big; (void)int_len; (void)frc_len;  return NULL; }     atom_t*array_trim_leading_zeroes_simple(const atom_t*const bn,const uint16_t len,uint16_t*const out_len){ atom_t*const z=zalloc(atom_t,1); const uint16_t count_leading_zeroes=array_span(bn,len,true,z,1); free(z); const uint16_t nonzeroes=(uint16_t)(len-count_leading_zeroes); set_out_param(out_len,nonzeroes); return(atom_t*)memcpy(alloc(atom_t,nonzeroes),bn+count_leading_zeroes,nonzeroes); } char*strndup_c(const char*const s,size_t const n){ const size_t len=strnlen_c(s,n); char*const news=alloc(char,len+1); if(NULL==news){ return NULL; } news[len]='\0'; return(char*)memcpy(news,s,len); } size_t strnlen_c(const char*const s,const size_t maxsize){ if(NULL==s){ return 0; } size_t i=0; for(;i<maxsize;i++){ if('\0'==s[i]){return i;} } return i; } size_t str_count(const char*const str,const char find){ size_t ocur=0; const size_t len=strnlen_c(str,MAX_STR_LDBL_DIGITS); for(size_t i=0;i<len;i++){ if(find==str[i]){++ocur;} } return ocur; } void _say_atom_t_ptr(const atom_t*const data,const uint16_t len){ printf("atom_t ptr%p+%d: ",(const void*const)data,len); for(uint16_t i=0;i<len;i++){ printf("%d ",data[i]); } printf(";\n"); }
 #endif 
 
 #endif
 
