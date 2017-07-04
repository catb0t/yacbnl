#include<criterion/criterion.h>
#include"yacbnl.h"
Test(a1b10,bna_u64){atom_t*a=to_digit_array(0,0,FL_NONE,TYP_NONE),z[HEADER_OFFSET]={TYP_NONE,0,0,0};cr_assert_arr_eq(z,a,sz(HEADER_OFFSET,atom_t));free(a);a=to_digit_array(0,1,FL_NONE,TYP_NONE);atom_t s[HEADER_OFFSET+1]={TYP_NONE,1,0,0,1};cr_assert_arr_eq(s,a,sz(1+HEADER_OFFSET,atom_t));free(a);a=to_digit_array(0,23,FL_NONE,TYP_NONE);atom_t s2[HEADER_OFFSET+2]={TYP_NONE,2,0,FL_NONE,2,3};cr_assert_arr_eq(s2,a,sz(2+HEADER_OFFSET,atom_t));free(a);a=to_digit_array(0,12345678901234567890U,FL_NONE,TYP_NONE);atom_t s3[HEADER_OFFSET+20]={TYP_NONE,20,0,FL_NONE,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0};cr_assert_arr_eq(s3,a,sz(20+HEADER_OFFSET,atom_t));free(a);}Test(a1b10,bna_s64){atom_t*a=to_digit_array(0,0,FL_SIGN,TYP_NONE),z[HEADER_OFFSET]={TYP_NONE,0,0,FL_SIGN};cr_assert_arr_eq(z,a,sz(HEADER_OFFSET,atom_t));free(a);a=to_digit_array(0,1,FL_SIGN,TYP_NONE);atom_t n1[HEADER_OFFSET+4]={TYP_NONE,1,0,FL_SIGN,1};cr_assert_arr_eq(n1,a,sz(1+HEADER_OFFSET,atom_t));free(a);a=to_digit_array(0,2345,FL_SIGN,TYP_NONE);atom_t n2[HEADER_OFFSET+4]={TYP_NONE,4,0,FL_SIGN,2,3,4,5};cr_assert_arr_eq(n2,a,sz(4+HEADER_OFFSET,atom_t));free(a);}Test(a1b10,bna_ldbl){atom_t*a=to_digit_array((ldbl_t)1.234,0,FL_NONE,TYP_NONE),z[HEADER_OFFSET+4]={TYP_NONE,1,3,FL_NONE,1,2,3,4};for(uint16_t i=0;i<bna_real_len(a);i++){printf("%d ",a[i]);}cr_assert_arr_eq(z,a,sz(4+HEADER_OFFSET,atom_t));free(a);a=to_digit_array(153452.2352344,0,FL_SIGN,TYP_NONE);atom_t y[HEADER_OFFSET+6+7]={TYP_NONE,6,7,FL_SIGN,1,5,3,4,5,2,2,3,5,2,3,4,4};for(uint16_t i=0;i<bna_real_len(a);i++){printf("%d ",a[i]);}cr_assert_arr_eq(y,a,sz(6+7+HEADER_OFFSET,atom_t));free(a);}Test(addrmodes,1to2){atom_t h,l;samb_u16_to_twoba(2340,&h,&l);cr_assert(h==9&&l==36);samb_u16_to_twoba(65533,&h,&l);cr_assert(h==255&&l==253);samb_u16_to_twoba(65535,&h,&l);cr_assert_eq(h,l);cr_assert(h==255);samb_u16_to_twoba(256,&h,&l);cr_assert(h==1&&l==0);samb_u16_to_twoba(257,&h,&l);cr_assert_eq(h,l);cr_assert(h==1);samb_u16_to_twoba(0,&h,&l);cr_assert_eq(h,l);cr_assert(h==0);}Test(addrmodes,2to1){cr_assert_eq(2340,samb_twoba_to_u16(9,36));cr_assert_eq(65533,samb_twoba_to_u16(255,253));cr_assert_eq(65535,samb_twoba_to_u16(255,255));cr_assert_eq(256,samb_twoba_to_u16(1,0));cr_assert_eq(257,samb_twoba_to_u16(1,1));cr_assert_eq(0,samb_twoba_to_u16(0,0));}Test(common,countint){cr_assert_eq(1,count_digits_u64(1));cr_assert_eq(2,count_digits_u64(20));cr_assert_eq(6,count_digits_u64(200000));cr_assert_eq(20,count_digits_u64(12345678901234567890U));}Test(common,idx){cr_assert_eq(0,indexable_digits_u64(1));cr_assert_eq(1,indexable_digits_u64(20));cr_assert_eq(4,indexable_digits_u64(12345));cr_assert_eq(5,indexable_digits_u64(200000));cr_assert_eq(19,indexable_digits_u64(12345678901234567890U));}Test(common,countfrac){cr_assert_eq(0,count_frac_digits("1234545"));cr_assert_eq(1,count_frac_digits("1234545.4"));cr_assert_eq(2,count_frac_digits("12345123.22"));cr_assert_eq(7,count_frac_digits("153452.2352344"));}Test(common,getn){uint64_t x=12345;for(size_t i=0;i<5;i++){atom_t res=get_left_nth_digit(x,(atom_t)i);cr_assert_eq(i+1,res);}x=12345678901234567890U;for(size_t i=0;i<20;i++){cr_assert_eq((i+1)%10,get_left_nth_digit(x,(atom_t)i));}}Test(metadata,macros){atom_t a[HEADER_OFFSET+3]={TYP_NONE,1,2,FL_SIGN,1,4,5};cr_assert_eq(bna_real_len(a),HEADER_OFFSET+3);atom_t b[HEADER_OFFSET_BIG+39]={TYP_BIG,0,5,0,34,BN_SINF};cr_assert_eq(bna_real_len(b),HEADER_OFFSET_BIG+39);}Test(metadata,basic){atom_t*m=make_array_header(TYP_NONE,0,0,FL_NONE),a[HEADER_OFFSET_BIG]={TYP_NONE,0,0,FL_NONE};cr_assert_arr_eq(m,a,sz(HEADER_OFFSET,atom_t));free(m);m=make_array_header(TYP_NONE,3,67,FL_SIGN);atom_t b[HEADER_OFFSET]={TYP_NONE,3,67,FL_SIGN};cr_assert_arr_eq(m,b,sz(HEADER_OFFSET,atom_t));free(m);m=make_array_header(TYP_BIG,300,670,FL_SIGN);atom_t c[HEADER_OFFSET_BIG]={TYP_BIG,1,44,2,158,FL_SIGN};cr_assert_arr_eq(m,c,sz(HEADER_OFFSET_BIG,atom_t));free(m);m=make_array_header(TYP_BIG,65535,65535,FL_SIGN);atom_t d[HEADER_OFFSET_BIG]={TYP_BIG,0xFF,0xFF,0xFF,0xFF,FL_SIGN};cr_assert_arr_eq(m,d,sz(HEADER_OFFSET_BIG,atom_t));free(m);m=make_array_header(TYP_BIG|TYP_ZENZ,65535,65535,FL_SIGN|FL_NAN);atom_t e[HEADER_OFFSET_BIG]={TYP_BIG|TYP_ZENZ,0xFF,0xFF,0xFF,0xFF,FL_SIGN|FL_NAN};cr_assert_arr_eq(m,e,sz(HEADER_OFFSET_BIG,atom_t));free(m);m=make_array_header(TYP_NONE,6,7,FL_SIGN);atom_t f[HEADER_OFFSET]={TYP_NONE,6,7,FL_SIGN};cr_assert_arr_eq(m,f,sz(HEADER_OFFSET,atom_t));free(m);}
