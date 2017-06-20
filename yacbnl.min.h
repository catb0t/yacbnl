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
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>

#ifndef BN_COMMON_H
#define BN_COMMON_H

#define HEADER_OFFSET ((atom_t) 4)
#define HEADER_OFFSET_BIG ((atom_t) 6)

#ifndef MAX_SIGFIGS
#define MAX_SIGFIGS ((UINT8_MAX * 2) - (HEADER_OFFSET * 2))
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
typedef long double ldbl_t;typedef uint8_t atom_t;typedef struct st_bignum_t{atom_t*value,*imgry,*fracl,*vextn;struct st_bignum_t*expt;}bignum_t;
#define sz(n, type) ( ((size_t) n) * (sizeof (type)) )
#define alloc(size, type) malloc(( (size_t) size) * sizeof (type))
#define zalloc(size, type) calloc(( (size_t) size), sizeof (type))
#define bna_real_len(bna) ((bna)[0] + (bna)[1] + HEADER_OFFSET)
#define macrogetval(x) #x
#define stringify(x) macrogetval(x)

#define B256_HIGH 256
#define B10_HIGH 10

#define TYP_BIG 0x01 
#define TYP_ZENZ 0x02 

#define FL_SIGN 0x01
#define FL_NAN 0x02
#define FL_INF 0x04
typedef enum{BN_NONE=0,BN_SIGN=FL_SIGN,BN_SNAN=FL_SIGN|FL_NAN,BN_SINF=FL_SIGN|FL_INF,BN_NAN=FL_NAN,BN_INF=FL_INF}bignum_flag_t;float log256f(const float x);bool compare_eps(const ldbl_t a,const ldbl_t b,const ldbl_t eps);atom_t count_digits_u64(const uint64_t x);atom_t indexable_digits_u64(const uint64_t x);atom_t count_b256_digits_u64(const uint64_t x);atom_t get_left_nth_digit(const uint64_t x,const atom_t n);atom_t count_frac_digits(const char*const str);atom_t find_frac_beginning(const char*const str);atom_t*array_concat(const atom_t*const a,const atom_t*const b,const uint16_t a_len,const uint16_t b_len);atom_t*array_reverse(const atom_t*const arr,const uint16_t len);char*str_reverse(const char*const str);bignum_t*bignum_ctor(const ldbl_t ldbl,const uint64_t u64,const atom_t flags,const bignum_t*const*const opt_vals);bignum_t*bignum_copy(const bignum_t*const bn,const bool no_recurse_optionals);atom_t*to_digit_array(const ldbl_t ldbl_in,const uint64_t u64,const atom_t value_flags,const atom_t metadata);void u16_to_twoba(const uint16_t n,atom_t*const ah,atom_t*const al);uint16_t twoba_to_u16(const atom_t ah,const atom_t al);uint64_t b256_to_u64_digits(const atom_t*const digits,const uint16_t len);atom_t*ldbl_digits_to_b256(const char*const ldbl_digits,uint16_t*const len,uint16_t*const int_len);atom_t*u64_digits_to_b256(const uint64_t value,uint16_t*const len,const bool little_endian);
#endif 
#ifndef MISC_UTIL_H
#define MISC_UTIL_H
bool compare_eps(const ldbl_t a,const ldbl_t b,const ldbl_t eps){return fabsl(a-b)<eps;}atom_t count_digits_u64(const uint64_t x){return(atom_t)floor(log10f((float)x)+1.f);}atom_t find_frac_beginning(const char*const str){const atom_t pre_len=(atom_t)strcspn(str,".");return(atom_t)(1U+(unsigned)pre_len);}atom_t count_frac_digits(const char*const str){return(atom_t)strspn(str+find_frac_beginning(str),"0123456789");}atom_t indexable_digits_u64(const uint64_t x){return(atom_t)floor(log10f((float)x));}atom_t get_left_nth_digit(const uint64_t x,const atom_t n){
#ifdef PREFER_CHAR_CONV
char*const str=alloc(22,char);snprintf(str,21,"%" PRIu64 "",x);char d=str[n];free(str);return(atom_t)((unsigned)d-'0');
#else 
const ldbl_t tpow=pow(10,indexable_digits_u64(x)-n),ldivr=((ldbl_t)x)/tpow;return(atom_t)(((uint64_t)ldivr)%10);
#endif 
}char*str_reverse(const char*const str){if(NULL==str){return NULL;}size_t len=strnlen(str,MAX_SIGFIGS);char*newp;if(len<2){newp=strndup(str,len);}else{newp=alloc(len+1,char);size_t i,j;for(i=0,j=len-1;i<len;i++,j--){newp[i]=str[j];}newp[i]='\0';}return newp;}float log256f(const float x){return logf(x)/logf(256);}atom_t count_b256_digits_u64(const uint64_t x){return(atom_t)floorf(log256f((float)x)+1.f);}atom_t*array_reverse(const atom_t*const arr,const uint16_t len){atom_t*result=zalloc(len,atom_t);if(len){for(uint16_t i=0;i<len;i++){result[i]=arr[ i-len ];}}return result;}atom_t*array_concat(const atom_t*const a,const atom_t*const b,const uint16_t a_len,const uint16_t b_len){if(!(a_len+b_len)){return alloc(0,atom_t);}else if(a_len==a_len+b_len){return memcpy(alloc(a_len,atom_t),a,a_len);}else if(b_len==a_len+b_len){return memcpy(alloc(b_len,atom_t),b,b_len);}atom_t*const res=memcpy(alloc(a_len+b_len,atom_t),a,a_len);return memcpy(res+a_len,b,b_len);}
#endif 
void u16_to_twoba(const uint16_t n,atom_t*const ah,atom_t*const al){*ah=(atom_t)(n>>(atom_t)CHAR_BIT);*al=(atom_t)(n&(atom_t)0xFF);}uint16_t twoba_to_u16(const atom_t ah,const atom_t al){return(uint16_t)((ah<<CHAR_BIT)|(atom_t)al);}atom_t*u64_digits_to_b256(const uint64_t value,uint16_t*const len,const bool little_endian){if(!value||NULL==len){return zalloc(1,atom_t);}atom_t*result=zalloc(count_b256_digits_u64(value),atom_t);uint64_t tempr=value;uint16_t i=0;for(;tempr;i++){result[i]=(atom_t)tempr%B256_HIGH;tempr=(uint64_t)floorl(tempr/B256_HIGH);if(!tempr){break;}}*len=i+1;if(little_endian){atom_t*reversed=array_reverse(result,*len);free(result);return reversed;}else{return result;}}uint64_t b256_to_u64_digits(const atom_t*const digits,const uint16_t len){if(!len){return 0;}uint64_t result=0;for(uint16_t i=len;i!=0;i--){result+=digits[i]*((uint64_t)powl(256,i));}return result;}atom_t*ldbl_digits_to_b256(const char*const ldbl_digits,uint16_t*const len,uint16_t*const int_len){uint16_t pre_dec=(uint16_t)strcspn(ldbl_digits,".");char*const int_part=strndup(ldbl_digits,pre_dec),*const flot_part=str_reverse(ldbl_digits+pre_dec+1);const uint64_t lhs=strtoull(int_part,NULL,B10_HIGH),rhs=strtoull(flot_part,NULL,B10_HIGH);uint16_t lhs_len,rhs_len;atom_t*const lhs_b256=u64_digits_to_b256(lhs,&lhs_len,true),*const rhs_b256=u64_digits_to_b256(rhs,&rhs_len,false);*len=lhs_len+rhs_len;*int_len=lhs_len;atom_t*final_concat=array_concat(lhs_b256,rhs_b256,lhs_len,rhs_len);free(lhs_b256),free(rhs_b256);return final_concat;}
#ifndef BNA_H
#define BNA_H

#define meta_is_base256(metadata) (metadata & TYP_ZENZ)
#define meta_is_big(metadata) (metadata & TYP_BIG)
#define meta_header_offset(metadata) (meta_is_big(metadata) ? HEADER_OFFSET_BIG : HEADER_OFFSET)
static atom_t*impl_to_digit_array_ldbl(const ldbl_t ldbl,const atom_t metadata,const atom_t flags);static atom_t*impl_to_digit_array_u64(const uint64_t u64,const atom_t metadata,const atom_t flags);static atom_t*make_array_header(const atom_t metadata,const uint16_t int_digits,const uint16_t flot_digits,const atom_t flags){const atom_t hdrlen=meta_header_offset(metadata);atom_t*const header=zalloc(hdrlen,atom_t);header[0]=metadata;header[hdrlen-1]=flags;if(meta_is_big(metadata)){atom_t lens[]={0,0,0,0};u16_to_twoba(int_digits,lens+0,lens+1);u16_to_twoba(flot_digits,lens+2,lens+3);memcpy(header+1,&lens,sz(4,atom_t));}else{header[1]=(atom_t)int_digits;header[2]=(atom_t)flot_digits;}return header;}atom_t*to_digit_array(const ldbl_t ldbl_in,const uint64_t u64,const atom_t value_flags,const atom_t metadata){const atom_t flags=ldbl_in<0 ? value_flags|FL_SIGN : value_flags;const ldbl_t ldbl=ldbl_in<0 ? fabsl(ldbl_in): ldbl_in;if(!compare_eps(ldbl,0.f,1e-11)){return impl_to_digit_array_ldbl(ldbl,metadata,flags);}else if(0!=u64){return impl_to_digit_array_u64(u64,metadata,flags);}else{return make_array_header(metadata,0,0,flags);}return NULL;}static atom_t*impl_to_digit_array_ldbl(const ldbl_t ldbl,const atom_t metadata,const atom_t flags){const atom_t hdrlen=meta_header_offset(metadata);const bool using_base256=metadata&TYP_ZENZ;char*const fullstr=alloc(MAX_SIGFIGS+3,char);snprintf(fullstr,MAX_SIGFIGS+1,"%." stringify(PRIMITIVE_PRECISION)"LG",ldbl);printf("%s%zu \n",fullstr,strnlen(fullstr,PRIMITIVE_PRECISION));const atom_t
#ifdef PREFER_CHAR_CONV
nint_digits=(atom_t)strcspn(fullstr,"."),
#else 
nint_digits=count_digits_u64((uint64_t)floorl(ldbl)),
#endif 
nflot_digits=count_frac_digits(fullstr);atom_t*bn_tlated=alloc(nint_digits+nflot_digits+hdrlen,atom_t),*const init=make_array_header(metadata,nint_digits,nflot_digits,flags);memcpy(bn_tlated,&init,sz(hdrlen,atom_t));if(using_base256){}else{
#ifdef PREFER_CHAR_CONV
char*const integ_str=strndup(fullstr,nint_digits);for(atom_t i=0;i<nint_digits;i++){bn_tlated[i+hdrlen]=(atom_t)((unsigned)integ_str[i]-'0');}free(integ_str);
#else 
for(atom_t i=0;i<nint_digits;i++){bn_tlated[i+hdrlen]=get_left_nth_digit((uint64_t)floorl(ldbl),i);}
#endif 
const char*const frac_str=fullstr+find_frac_beginning(fullstr);for(size_t i=0;i<nflot_digits;i++){bn_tlated[i+hdrlen+nint_digits]=(atom_t)((unsigned)frac_str[i]-'0');}}free(init),free(fullstr);return bn_tlated;}static atom_t*impl_to_digit_array_u64(const uint64_t u64,const atom_t metadata,const atom_t flags){const bool using_base256=metadata&TYP_ZENZ;const atom_t ndigits=count_digits_u64(u64),hdrlen=meta_header_offset(metadata);atom_t*bn_tlated=alloc(ndigits+hdrlen,atom_t),*const init=make_array_header(metadata,ndigits,0,flags);memcpy(bn_tlated,init,sz(hdrlen,atom_t));if(using_base256){char*const str=alloc(ndigits+2,char);snprintf(str,21,"%" PRIu64 "",u64);free(str);}else{
#ifdef PREFER_CHAR_CONV
char*const str=alloc(ndigits+2,char);snprintf(str,21,"%" PRIu64 "",u64);for(atom_t i=0;i<ndigits;i++){bn_tlated[i+hdrlen]=(atom_t)((unsigned)str[i]-'0');}free(str);
#else 
for(atom_t i=0;i<ndigits;i++){bn_tlated[i+hdrlen]=get_left_nth_digit(u64,i);}
#endif 
}free(init);return bn_tlated;}
#endif 

#ifndef BIGNUM_H
#define BIGNUM_H
bignum_t*bignum_ctor(const ldbl_t ldbl,const uint64_t u64,const atom_t flags,const bignum_t*const*const opt_vals){bool cx,fr,ex;if(NULL!=opt_vals){cx=NULL!=opt_vals[0];fr=NULL!=opt_vals[1];ex=NULL!=opt_vals[2];}else{cx=fr=ex=0;}bignum_t st_bn={.value=to_digit_array(ldbl,u64,flags,0),.imgry=cx? memcpy(alloc(bna_real_len(opt_vals[0]->value),atom_t),opt_vals[0]->value,sz(bna_real_len(opt_vals[0]->value),atom_t)): zalloc(HEADER_OFFSET,atom_t),.fracl=fr? memcpy(alloc(bna_real_len(opt_vals[1]->value),atom_t),opt_vals[1]->value,sz(bna_real_len(opt_vals[1]->value),atom_t)): zalloc(HEADER_OFFSET,atom_t),.expt=ex? bignum_copy(opt_vals[2],true): bignum_ctor(0,0,0,NULL)};bignum_t*hp_bn=memcpy(alloc(1,bignum_t),&st_bn,sizeof(bignum_t));return hp_bn;}bignum_t*bignum_copy(const bignum_t*const bn,const bool no_recurse_optionals){if(NULL==bn){return zalloc(1,bignum_t);}(void)no_recurse_optionals;return NULL;}
#endif 
