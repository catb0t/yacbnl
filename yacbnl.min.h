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

#define HEADER_OFFSET ((atom_t) 3)

#ifndef MAX_SIGFIGS
#define MAX_SIGFIGS ((UINT8_MAX * 2) - (HEADER_OFFSET * 2))
#endif

#ifdef PREFER_CHAR_CONV
#pragma message("preferring slower O(n) char operations over O(1) floating point math")
#endif

#define sz(n, type) ( ((size_t) n) * (sizeof (type)) )
#define alloc(size, type) malloc(( (size_t) size) * sizeof (type))
#define zalloc(size, type) calloc(( (size_t) size), sizeof (type))
#define bna_real_len(bna) ((bna)[0] + (bna)[1] + HEADER_OFFSET)

#define FL_SIGN 0x01
#define FL_NAN 0x02
#define FL_INF 0x04
typedef enum{BN_NONE=0,BN_SIGN=FL_SIGN,BN_SNAN=FL_SIGN|FL_NAN,BN_SINF=FL_SIGN|FL_INF,BN_NAN=FL_NAN,BN_INF=FL_INF}bignum_flag_t;typedef long double ldbl_t;typedef uint8_t atom_t;typedef struct st_bignum_t{atom_t*value,*imgry,*fracl,*vextn;struct st_bignum_t*expt;}bignum_t;bignum_t*bignum_ctor(const ldbl_t ldbl,const uint64_t u64,const atom_t flags,const bignum_t*const*const opt_vals);bignum_t*bignum_copy(const bignum_t*const bn,const bool no_recurse_optionals);atom_t count_digits_u64(const uint64_t x);atom_t indexable_digits_u64(const uint64_t x);atom_t get_left_nth_digit(const uint64_t x,const atom_t n);atom_t count_frac_digits(const char*const str);atom_t find_frac_beginning(const char*const str);atom_t*to_bn_array(const ldbl_t ldbl,const uint64_t u64,const atom_t flags);bool compare_eps(const ldbl_t a,const ldbl_t b,const ldbl_t eps);
#endif
#ifndef BIGNUM_H
#define BIGNUM_H
bignum_t*bignum_ctor(const ldbl_t ldbl,const uint64_t u64,const atom_t flags,const bignum_t*const*const opt_vals){bool cx,fr,ex;if(NULL!=opt_vals){cx=NULL!=opt_vals[0];fr=NULL!=opt_vals[1];ex=NULL!=opt_vals[2];}else{cx=fr=ex=0;}bignum_t st_bn={.value=to_bn_array(ldbl,u64,flags),.imgry=cx? memcpy(alloc(bna_real_len(opt_vals[0]->value),atom_t),opt_vals[0]->value,sz(bna_real_len(opt_vals[0]->value),atom_t)): zalloc(HEADER_OFFSET,atom_t),.fracl=fr? memcpy(alloc(bna_real_len(opt_vals[1]->value),atom_t),opt_vals[1]->value,sz(bna_real_len(opt_vals[1]->value),atom_t)): zalloc(HEADER_OFFSET,atom_t),.expt=ex? bignum_copy(opt_vals[2],true): bignum_ctor(0,0,0,NULL)};bignum_t*hp_bn=memcpy(alloc(1,bignum_t),&st_bn,sizeof(bignum_t));return hp_bn;}bignum_t*bignum_copy(const bignum_t*const bn,const bool no_recurse_optionals){if(NULL==bn){return zalloc(1,bignum_t);}(void)no_recurse_optionals;return NULL;}
#endif
#ifndef BNA_H
#define BNA_H
static atom_t*impl_to_bn_array_ldbl(const ldbl_t ldbl,const atom_t flags);static atom_t*impl_to_bn_array_u64(const uint64_t u64,const atom_t flags);bool compare_eps(const ldbl_t a,const ldbl_t b,const ldbl_t eps){return fabsl(a-b)<eps;}atom_t count_digits_u64(const uint64_t x){return(atom_t)floor(log10f((float)x)+1.f);}atom_t find_frac_beginning(const char*const str){const atom_t pre_len=(atom_t)strcspn(str,".");return(atom_t)(1U+(unsigned)pre_len);}atom_t count_frac_digits(const char*const str){return(atom_t)strspn(str+find_frac_beginning(str),"0123456789");}atom_t indexable_digits_u64(const uint64_t x){return(atom_t)floor(log10f((float)x));}atom_t get_left_nth_digit(const uint64_t x,const atom_t n){
#ifdef PREFER_CHAR_CONV
char*const str=alloc(21,char);snprintf(str,20,"%" PRIu64 "",x);char d=str[n];free(str);return(atom_t)((unsigned)d-'0');
#else
const ldbl_t tpow=pow(10,indexable_digits_u64(x)-n),ldivr=((ldbl_t)x)/tpow;return(atom_t)(((uint64_t)ldivr)%10);
#endif
}
#define bna_from_uint(num, flags) (to_bn_array(0.f, (uint64_t) num, flags))
#define bna_from_flot(flot, flags) (to_bn_array((ldbl_t) flot, 0, flags))
atom_t*to_bn_array(const ldbl_t ldbl_in,const uint64_t u64,const atom_t flags_in){const atom_t flags=ldbl_in<0 ? flags_in|FL_SIGN : flags_in;const ldbl_t ldbl=ldbl_in<0 ? fabsl(ldbl_in): ldbl_in;if(!compare_eps(ldbl,0.f,1e-11)){return impl_to_bn_array_ldbl(ldbl,flags);}else if(0!=u64){return impl_to_bn_array_u64(u64,flags);}else{const atom_t zero[]={0,0,flags};return memcpy(alloc(HEADER_OFFSET,atom_t),&zero,sz(HEADER_OFFSET,atom_t));}return NULL;}static atom_t*impl_to_bn_array_ldbl(const ldbl_t ldbl,const atom_t flags){char*const fullstr=alloc(MAX_SIGFIGS+3,char);snprintf(fullstr,MAX_SIGFIGS+1,"%.252LG",ldbl);const atom_t
#ifdef PREFER_CHAR_CONV
nint_digits=(atom_t)strcspn(fullstr,"."),
#else
nint_digits=count_digits_u64((uint64_t)floorl(ldbl)),
#endif
nflot_digits=count_frac_digits(fullstr),init[HEADER_OFFSET]={nint_digits,nflot_digits,flags};atom_t*bn_tlated=alloc(nint_digits+nflot_digits+HEADER_OFFSET,atom_t);memcpy(bn_tlated,&init,sz(HEADER_OFFSET,atom_t));
#ifdef PREFER_CHAR_CONV
char*const integ_str=strndup(fullstr,nint_digits);for(atom_t i=0;i<nint_digits;i++){bn_tlated[i+HEADER_OFFSET]=(atom_t)((unsigned)integ_str[i]-'0');}free(integ_str);
#else
for(atom_t i=0;i<nint_digits;i++){bn_tlated[i+HEADER_OFFSET]=get_left_nth_digit((uint64_t)floorl(ldbl),i);}
#endif
const char*const frac_str=fullstr+find_frac_beginning(fullstr);for(size_t i=0;i<nflot_digits;i++){bn_tlated[i+HEADER_OFFSET+nint_digits]=(atom_t)((unsigned)frac_str[i]-'0');}free(fullstr);return bn_tlated;}static atom_t*impl_to_bn_array_u64(const uint64_t u64,const atom_t flags){const atom_t ndigits=count_digits_u64(u64),init[HEADER_OFFSET]={ndigits,0,flags};atom_t*bn_tlated=alloc(ndigits+HEADER_OFFSET,atom_t);memcpy(bn_tlated,&init,sz(HEADER_OFFSET,atom_t));
#ifdef PREFER_CHAR_CONV
char*const str=alloc(ndigits+2,char);snprintf(str,21,"%" PRIu64 "",u64);for(atom_t i=0;i<ndigits;i++){bn_tlated[i+HEADER_OFFSET]=(atom_t)((unsigned)str[i]-'0');}free(str);
#else
for(atom_t i=0;i<ndigits;i++){bn_tlated[i+HEADER_OFFSET]=get_left_nth_digit(u64,i);}
#endif
return bn_tlated;}
#endif
