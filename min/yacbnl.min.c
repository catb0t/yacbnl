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
 
 #define alloc(type, size) malloc(( (size_t) size) * sizeof (type))
 
 #define zalloc(type, size) calloc(( (size_t) size), sizeof (type))
 #define macrogetval(x) #x
 #define stringify(x) macrogetval(x)
   typedef enum{ BN_NONE=0,  BN_SIGN=FL_SIGN, BN_SNAN=FL_SIGN|FL_NAN, BN_SINF=FL_SIGN|FL_INF,  BN_NAN=FL_NAN, BN_INF=FL_INF  }bignum_flag_t;   float log256f(const float x); bool compare_eps(const ldbl_t a,const ldbl_t b,const ldbl_t eps); atom_t count_digits_u64(const uint64_t x); atom_t indexable_digits_u64(const uint64_t x); atom_t count_b256_digits_u64(const uint64_t x); atom_t get_left_nth_digit(const uint64_t x,const atom_t n); atom_t count_frac_digits(const char*const str); atom_t find_frac_beginning(const char*const str);  size_t strnlen_c(const char*const s,const size_t maxsize); char*strndup_c(const char*const s,size_t const n); char*str_reverse(const char*const str); char*make_empty_string(void);  uint16_t array_spn(const atom_t*arr,const uint16_t arr_len,const uint16_t accept_num,const atom_t accept_only,...); uint16_t array_cspn(const atom_t*arr,const uint16_t arr_len,const uint16_t reject_num,const atom_t reject_only,...);  atom_t*array_concat(const atom_t*const a,const atom_t*const b,const uint16_t a_len,const uint16_t b_len); atom_t*array_reverse(const atom_t*const arr,const uint16_t len); atom_t*array_trim_trailing_zeroes(const atom_t*const bn); atom_t*array_trim_leading_zeroes(const atom_t*const bn);    bignum_t*bignum_ctor(const ldbl_t ldbl,const uint64_t u64,const atom_t flags,const bignum_t*const*const opt_vals); bignum_t*bignum_copy(const bignum_t*const bn,const bool no_recurse_optionals);   atom_t*make_array_header(const atom_t metadata,const uint16_t int_digits,const uint16_t flot_digits,const atom_t flags); atom_t*to_digit_array(const ldbl_t ldbl_in,const uint64_t u64,const atom_t value_flags,const atom_t metadata);   void samb_u16_to_twoba(const uint16_t n,atom_t*const ah,atom_t*const al); uint16_t samb_twoba_to_u16(const atom_t ah,const atom_t al); uint16_t samb_twoarray_to_u16(const atom_t arr[static 2]);   char*b256_to_ldbl_digits(const atom_t*const digits,const uint16_t len,const uint16_t int_len,uint16_t*const out_int_len); uint64_t b256_to_u64_digits(const atom_t*const digits,const uint16_t len); atom_t*ldbl_digits_to_b256(const char*const ldbl_digits,uint16_t*const len,uint16_t*const int_len,const bool little_endian); atom_t*u64_digits_to_b256(const uint64_t value,uint16_t*const len,const bool little_endian); 
 #endif 
 
 #ifndef BNA_H
 #define BNA_H
 static atom_t*impl_to_digit_array_ldbl(const ldbl_t ldbl,const atom_t metadata,const atom_t flags); static atom_t*impl_to_digit_array_u64(const uint64_t u64,const atom_t metadata,const atom_t flags);  atom_t*make_array_header(const atom_t metadata,const uint16_t int_digits,const uint16_t flot_digits,const atom_t flags){ const atom_t hdrlen=meta_header_offset(metadata); atom_t*const header=zalloc(atom_t,hdrlen);  header[0]=metadata;  header[hdrlen-1]=flags; if(meta_is_big(metadata)){ atom_t lens[]={0,0,0,0};  samb_u16_to_twoba(int_digits,lens+0,lens+1); samb_u16_to_twoba(flot_digits,lens+2,lens+3);  memcpy(header+1,&lens,sz(atom_t,4)); }else{ header[1]=(atom_t)int_digits; header[2]=(atom_t)flot_digits; } return header; }         atom_t*to_digit_array(const ldbl_t ldbl_in,const uint64_t u64,const atom_t value_flags,const atom_t metadata){ const atom_t flags=ldbl_in<0 ? value_flags|FL_SIGN : value_flags; const ldbl_t ldbl=ldbl_in<0 ? fabsl(ldbl_in): ldbl_in;  if(!compare_eps(ldbl,0.f,COMPARE_EPS)){ return impl_to_digit_array_ldbl(ldbl,metadata,flags);  }else if(0!=u64){ return impl_to_digit_array_u64(u64,metadata,flags); }else{ return make_array_header(metadata,0,0,flags); } return NULL; }         static atom_t*impl_to_digit_array_ldbl(const ldbl_t ldbl,const atom_t metadata,const atom_t flags){  const bool is_base256=meta_is_base256(metadata), is_big=meta_is_big(metadata);  const atom_t hdrlen=meta_header_offset(metadata);  const uint32_t sigfigs=is_big ? MAX_EXPORT_SIGFIGS_BIG : MAX_EXPORT_SIGFIGS;  char*const fullstr=alloc(char,sigfigs+3); snprintf(fullstr,sigfigs+2,"%LG",ldbl); const atom_t 
 #ifdef PREFER_CHAR_CONV
  nint_digits=(atom_t)strcspn(fullstr,"."),
 #else 
  nint_digits=count_digits_u64((uint64_t)floorl(ldbl)),
 #endif 
 nflot_digits=count_frac_digits(fullstr);    atom_t*bn_tlated=alloc(atom_t,nint_digits+nflot_digits+hdrlen), *const init=make_array_header(metadata,nint_digits,nflot_digits,flags);  memcpy(bn_tlated,init,sz(atom_t,hdrlen)); free(init); if(is_base256){  uint16_t len,int_len;  atom_t*as_digits=ldbl_digits_to_b256(fullstr,&len,&int_len,true); free(fullstr);  memcpy(bn_tlated+hdrlen,as_digits,len); }else{ 
 #ifdef PREFER_CHAR_CONV
  char*const integ_str=strndup_c(fullstr,nint_digits); for(atom_t i=0;i<nint_digits;i++){ bn_tlated[i+hdrlen]=(atom_t)((unsigned)integ_str[i]-'0'); } free(integ_str);
 #else 
  for(atom_t i=0;i<nint_digits;i++){ bn_tlated[i+hdrlen]=get_left_nth_digit((uint64_t)floorl(ldbl),i); }
 #endif 
       const char*const frac_str=fullstr+find_frac_beginning(fullstr); for(size_t i=0;i<nflot_digits;i++){ bn_tlated[i+hdrlen+nint_digits]=(atom_t)((unsigned)frac_str[i]-'0'); } } free(fullstr); return bn_tlated; }        static atom_t*impl_to_digit_array_u64(const uint64_t u64,const atom_t metadata,const atom_t flags){    const bool using_base256=meta_is_base256(metadata);   const atom_t ndigits=using_base256 ? count_b256_digits_u64(u64): count_digits_u64(u64), hdrlen=meta_header_offset(metadata);  atom_t*bn_tlated=alloc(atom_t,ndigits+hdrlen), *const init=make_array_header(metadata,ndigits,0,flags);  memcpy(bn_tlated,init,sz(atom_t,hdrlen)); free(init);  if(using_base256){  uint16_t len;  atom_t*as_digits=u64_digits_to_b256(u64,&len,true);  memcpy(bn_tlated+hdrlen,as_digits,len); free(as_digits); }else{
 #ifdef PREFER_CHAR_CONV
  char*const str=alloc(char,ndigits+2); snprintf(str,MAX_U64_DIGITS+1,"%" PRIu64 "",u64); for(atom_t i=0;i<ndigits;i++){ bn_tlated[i+hdrlen]=(atom_t)((unsigned)str[i]-'0'); } free(str); 
 #else 
 for(atom_t i=0;i<ndigits;i++){  bn_tlated[i+hdrlen]=get_left_nth_digit(u64,i); }
 #endif 
 } return bn_tlated; }
 #endif 
  
 #ifndef BASE256_H
 #define BASE256_H
           atom_t*u64_digits_to_b256(const uint64_t value,uint16_t*const len,const bool little_endian){ if(!value||NULL==len){ if(NULL!=len){ *len=1; } return zalloc(atom_t,1); } atom_t*const result=zalloc(atom_t,count_b256_digits_u64(value)); uint16_t i=0;  for(uint64_t tempr=value;tempr;i++){  result[i]=(atom_t)(tempr%B256_HIGH); tempr=(uint64_t)floorl(tempr/B256_HIGH);  if(!tempr){break;} } *len=(uint16_t)(i+1); if(little_endian){ atom_t*const reversed=array_reverse(result,*len); free(result); return reversed; }else{ return result; } }               atom_t*ldbl_digits_to_b256(const char*const ldbl_digits,uint16_t*const len,uint16_t*const int_len,const bool little_endian){ if((0==strnlen_c(ldbl_digits,MAX_U64_DIGITS+2))||NULL==ldbl_digits||NULL==len||NULL==int_len){ if(NULL!=len){ *len=1; } if(NULL!=int_len){ *int_len=1; } return zalloc(atom_t,1); }  const uint16_t pre_dec=(uint16_t)strcspn(ldbl_digits,".");  char*const int_part=strndup_c(ldbl_digits,pre_dec),  *const flot_part=str_reverse(ldbl_digits+pre_dec+1);     const uint64_t lhs=strtoull(int_part,NULL,B10_HIGH), rhs=strtoull(flot_part,NULL,B10_HIGH); if(EINVAL==errno||ERANGE==errno){ free(int_part),free(flot_part); return zalloc(atom_t,1); } free(int_part),free(flot_part); uint16_t lhs_len,rhs_len; atom_t*const lhs_b256=u64_digits_to_b256(lhs,&lhs_len,true),  *const rhs_b256=u64_digits_to_b256(rhs,&rhs_len,false); *len=(uint16_t)(lhs_len+rhs_len); *int_len=lhs_len; atom_t*const final_concat=array_concat(lhs_b256,rhs_b256,lhs_len,rhs_len); free(lhs_b256),free(rhs_b256); if(little_endian){ atom_t*const reversed=array_reverse(final_concat,*len); free(final_concat); return reversed; }else{ return final_concat; } }            uint64_t b256_to_u64_digits(const atom_t*const digits,const uint16_t len){ if(!len){ return 0; } uint64_t result=0; for(int16_t i=(int16_t)(len-1);i>-1;i--){ result+=digits[i]*((uint64_t)powl(ZENZ_BASE,i)); } return result; }      char*b256_to_ldbl_digits(const atom_t*const digits,const uint16_t len,const uint16_t int_len,uint16_t*const out_int_len){ if(NULL==digits||!len||NULL==out_int_len){ return make_empty_string(); }  const uint16_t flot_len=(uint16_t)(len-int_len);  atom_t*const int_part=memcpy(alloc(atom_t,int_len),digits,sz(atom_t,int_len)), *const flot_part=memcpy(alloc(atom_t,flot_len),digits+int_len,sz(atom_t,flot_len));  const uint64_t int_val=b256_to_u64_digits(int_part,int_len), flot_val=b256_to_u64_digits(flot_part,flot_len); free(int_part),free(flot_part);  const uint16_t int_len10=count_digits_u64(int_val), flot_len10=count_digits_u64(flot_val);  char*const flot_str=alloc(atom_t,flot_len10), *const finalval=alloc(atom_t,int_len10+flot_len10+1);  snprintf(flot_str,(uint16_t)(flot_len10+1),"%" PRIu64 "",flot_val);  char*const flot_str_be=str_reverse(flot_str); free(flot_str);  snprintf(finalval,(uint16_t)(int_len10+flot_len10+2),"%" PRIu64 ".%s",int_val,flot_str_be);  *out_int_len=count_digits_u64(int_val); return finalval; }
 #endif 
  
 #ifndef ADDR_INTERP_H
 #define ADDR_INTERP_H
        void samb_u16_to_twoba(const uint16_t n,atom_t*const ah,atom_t*const al){ *ah=(atom_t)(n>>(atom_t)8); *al=(atom_t)(n&(atom_t)0xFF); }      uint16_t samb_twoba_to_u16(const atom_t ah,const atom_t al){  return(uint16_t)((ah<<8)|(atom_t)al); } uint16_t samb_twoarray_to_u16(const atom_t arr[static 2]){ return samb_twoba_to_u16(arr[0],arr[1]); }
 #endif 
  
 #ifndef BIGNUM_H
 #define BIGNUM_H
     bignum_t*bignum_ctor( const ldbl_t ldbl, const uint64_t u64, const atom_t flags, const bignum_t*const*const opt_vals ){ bool cx,fr,ex; if(NULL!=opt_vals){ cx=NULL!=opt_vals[0]; fr=NULL!=opt_vals[1]; ex=NULL!=opt_vals[2]; }else{ cx=fr=ex=0; }  bignum_t st_bn={  .value=to_digit_array(ldbl,u64,flags,0),  .imgry=cx ? memcpy( alloc(atom_t,bna_real_len(opt_vals[0]->value)), opt_vals[0]->value, sz(atom_t,bna_real_len(opt_vals[0]->value)) ) : zalloc(atom_t,HEADER_OFFSET),  .fracl=fr ? memcpy( alloc(atom_t,bna_real_len(opt_vals[1]->value)), opt_vals[1]->value, sz(atom_t,bna_real_len(opt_vals[1]->value)) ) : zalloc(atom_t,HEADER_OFFSET),  .expt=ex ? bignum_copy(opt_vals[2],true) : bignum_ctor(0,0,0,NULL) };  bignum_t*hp_bn=memcpy(alloc(bignum_t,1),&st_bn,sizeof(bignum_t)); return hp_bn; }     bignum_t*bignum_copy(const bignum_t*const bn,const bool no_recurse_optionals){ if(NULL==bn){ return zalloc(bignum_t,1); } (void)no_recurse_optionals; return NULL; }
 #endif 
  
 #ifndef MISC_UTIL_H
 #define MISC_UTIL_H
  bool compare_eps(const ldbl_t a,const ldbl_t b,const ldbl_t eps){ return fabsl(a-b)<eps; }      atom_t count_digits_u64(const uint64_t x){ return(atom_t)floor(log10f((float)x)+1.f); }      atom_t find_frac_beginning(const char*const str){  const atom_t pre_len=(atom_t)strcspn(str,"."), len=(atom_t)strnlen_c(str,MAX_PRIMITIVE_LDBL_DIGITS), diff=(atom_t)(len-pre_len);  if(1==diff){ return pre_len; }else if(diff){ return(atom_t)(1+pre_len); }else{ return len; } }       atom_t count_frac_digits(const char*const str){ const atom_t begin=find_frac_beginning(str); if(1==strnlen_c(str,MAX_PRIMITIVE_LDBL_DIGITS)-begin){return 1;} return(atom_t)strspn(str+begin,"0123456789"); }       atom_t indexable_digits_u64(const uint64_t x){ return(atom_t)floor(log10f((float)x)); }          atom_t get_left_nth_digit(const uint64_t x,const atom_t n){
 #ifdef PREFER_CHAR_CONV
 char*const str=alloc(char,MAX_U64_DIGITS+2); snprintf(str,MAX_U64_DIGITS+1,"%" PRIu64 "",x); const char d=str[n]; free(str); return(atom_t)((unsigned)d-'0');
 #else 
 const ldbl_t tpow=pow(DEC_BASE,indexable_digits_u64(x)-n), ldivr=((ldbl_t)x)/tpow;  return(atom_t)(((uint64_t)ldivr)%DEC_BASE);
 #endif 
 }       char*str_reverse(const char*const str){ if(NULL==str){return NULL;} size_t len=strnlen_c(str,MAX_EXPORT_SIGFIGS); char*newp=NULL; if(len<2){ newp=strndup_c(str,len); }else{ newp=alloc(char,len+1); size_t i,j; for(i=0,j=len-1;i<len;i++,j--){ newp[i]=str[j];  } newp[i]='\0'; } return newp; }     float log256f(const float x){ return logf(x)/logf(ZENZ_BASE); }     atom_t count_b256_digits_u64(const uint64_t x){ return(atom_t)floorf(log256f((float)x)+1.f); }      atom_t*array_reverse(const atom_t*const arr,const uint16_t len){ atom_t*result=alloc(atom_t,len); if(len){ for(uint16_t i=0;i<len;i++){ result[i]=arr[(len-1)-i ]; } } return result; }      atom_t*array_concat(const atom_t*const a,const atom_t*const b,const uint16_t a_len,const uint16_t b_len){ if(!(a_len+b_len)){ return alloc(atom_t,0); } else if(a_len==a_len+b_len){ return memcpy(alloc(atom_t,a_len),a,a_len); }else if(b_len==a_len+b_len){ return memcpy(alloc(atom_t,b_len),b,b_len); } atom_t*const res=memcpy(alloc(atom_t,a_len+b_len),a,a_len); return memcpy(res+a_len,b,b_len); }     char*make_empty_string(void){ return zalloc(char,1); }        uint16_t array_spn(const atom_t*arr,const uint16_t arr_len,const uint16_t accept_num,const atom_t accept_only,...){ (void)arr;(void)arr_len;(void)accept_num;(void)accept_only; return 0; }        uint16_t array_cspn(const atom_t*arr,const uint16_t arr_len,const uint16_t reject_num,const atom_t reject_only,...){ (void)arr;(void)arr_len;(void)reject_num;(void)reject_only; return 0; }      atom_t*array_trim_trailing_zeroes(const atom_t*const bn){ const atom_t hdrlen=bna_header_offset(bn); const bool is_big=bna_is_big(bn); const uint16_t len=(uint16_t)bna_real_len(bn), int_len=bna_int_len(bn), frc_len=bna_frac_len(bn); atom_t const*rev_cpy=array_reverse(bn,len); uint16_t consec_zeroes=array_spn(rev_cpy,len,1,0); (void)hdrlen; (void)is_big; (void)int_len; (void)frc_len; (void)consec_zeroes; return NULL; }     atom_t*array_trim_leading_zeroes(const atom_t*const bn){ (void)bn; return NULL; } char*strndup_c(const char*const s,size_t const n){ const size_t len=strnlen_c(s,n); char*const news=alloc(char,len+1); if(NULL==news){ return NULL; } news[len]='\0'; return memcpy(news,s,len); } size_t strnlen_c(const char*const s,const size_t maxsize){ if(NULL==s){ return 0; } size_t i=0; for(;i<maxsize;i++){ if('\0'==s[i]){return i;} } return i; }
 #endif 
 
 #endif
 
