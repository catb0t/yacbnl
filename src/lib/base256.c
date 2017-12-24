#ifndef BASE256_H
#define BASE256_H

#include "bn_common.h"

/*
  uint64_t, uint16_t*, bool -> atom_t*

  transform a uint64_t to its base 256 representation

  the value at len is changed to the new length of the representation

  if little_endian is true, then the result is reversed

  if value was 0 or len was NULL, then a valid pointer to an array of value 0
    is returned

  if len was NULL, it is not changed into a valid pointer

  the return value is always a valid pointer
*/
atom_t* u64_digits_to_b256 (const uint64_t value, uint16_t* const len, const bool little_endian) {

  if (! value || NULL == len) {
    if (NULL != len) {
      *len = 1;
    }
    return zalloc(atom_t, 1);
  }

  atom_t* const result = zalloc(atom_t, count_b256_digits_u64(value));

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

/*
  char*, uint16_t*, uint16_t* -> atom_t*

  transform a long double to its base 256 representation

  the integer part is first and the fractional second

  the value at len     is changed to the new entire length of the representation
  the value at int_len is changed to the new length of the integer part, from
    which can be inferred the length of the fractional part

  if little_endian is true, then the result is reversed, and the fractional part
    occurs first

  if value was 0 or len was NULL, then a valid pointer to an array of value 0
    is returned

  if len was NULL, it is not changed into a valid pointer

  the return value is always a valid pointer
*/
atom_t* ldbl_digits_to_b256 (const char* const ldbl_digits, uint16_t* const len, uint16_t* const int_len, const bool little_endian) {

  if ( (0 == strnlen_c(ldbl_digits, MAX_U64_DIGITS + 2)) || NULL == ldbl_digits || NULL == len || NULL == int_len) {
    if (NULL != len) {
      *len = 1;
    }
    if (NULL != int_len) {
      *int_len = 1;
    }
    return zalloc(atom_t, 1);
  }

  /* integer part before the decimal point */
  const uint16_t pre_dec = (uint16_t) strcspn(ldbl_digits, ".");

  /* get the two parts of the number */
  char* const int_part  = strndup_c(ldbl_digits, pre_dec), // 1
      /* flip the significant digits */
      * const flot_part = str_reverse(ldbl_digits + pre_dec + /* skip separator */ 1); // 2

  /*
    convert the strings to numbers
    the error part is NULL
  */
  const uint64_t lhs = strtoull(int_part,  NULL, B10_HIGH),
                 rhs = strtoull(flot_part, NULL, B10_HIGH);

  if (EINVAL == errno || ERANGE == errno) {
    free(int_part), free(flot_part); // ~1, ~2
    return zalloc(atom_t, 1);
  }

  free(int_part), free(flot_part); // ~1, ~2

  uint16_t lhs_len, rhs_len;
  atom_t* const lhs_b256 = u64_digits_to_b256(lhs, &lhs_len, true),
        /* note argument #3: flip the fractional digits again */
        * const rhs_b256 = u64_digits_to_b256(rhs, &rhs_len, false);

  *len     = (uint16_t) (lhs_len + rhs_len);
  *int_len = lhs_len;

  atom_t* const final_concat = array_concat(lhs_b256, rhs_b256, lhs_len, rhs_len);

  free(lhs_b256), free(rhs_b256);

  if (little_endian) {
    atom_t* const reversed = array_reverse(final_concat, *len);
    free(final_concat);

    return reversed;
  } else {
    return final_concat;
  }
}

/*

    vvv little endian vvv

  the following functions expect and return data with the least significant part
    at the end, rather than at the beginning

  this is inline with how numbers are usually written in both base 10 and base 2
*/

/*
  atom_t*, uint16_t -> uint64_t

  get the base 10 value out of a base 256 representation

  if len is 0, 0 is returned
*/
uint64_t b256_to_u64_digits (const atom_t* const digits, const uint16_t len) {

  if (! len) {
    return 0;
  }

  uint64_t result = 0;

  for (int16_t i = (int16_t) (len - 1); i > -1; i--) {
    result += digits[i] * ( (uint64_t) powl(ZENZ_BASE, i) );
  }

  return result;
}

/*
  atom_t*, uint16_t, uint16_t, uint16_t* -> char*

  get the base 10 value of a base 256 representation of a long double

  if atom_t* has no len, then an empty string is returned, not "0"
*/
char* b256_to_ldbl_digits (const atom_t* const digits, const uint16_t len, const uint16_t int_len, uint16_t* const out_int_len) {

  if (NULL == digits || ! len || NULL == out_int_len) {
    return make_empty_string();
  }

  /* the length of the fractional part is gotten from subtraction */
  const uint16_t flot_len = (uint16_t) (len - int_len);

  /* copy the value's parts */
  atom_t * const int_part  = memcpy( alloc(atom_t, int_len),  digits,           sz(atom_t, int_len)), // 1
         * const flot_part = memcpy( alloc(atom_t, flot_len), digits + int_len, sz(atom_t, flot_len)); // 2

  /* grab the base 10 values from the copied parts */
  const uint64_t int_val  = b256_to_u64_digits(int_part, int_len),
                 flot_val = b256_to_u64_digits(flot_part, flot_len);
  free(int_part), free(flot_part); // ~1, ~2

  /* get the nunmber of digits required to make a string */
  const uint16_t int_len10  = count_digits_u64(int_val),
                 flot_len10 = count_digits_u64(flot_val);

  /* fractional part and final storage */
  char * const flot_str = alloc(atom_t, flot_len10),
       * const finalval = alloc(atom_t, int_len10 + flot_len10 + 1);

  /* just do the fractional part */
  snprintf(flot_str, (uint16_t) (flot_len10 + 1), "%" PRIu64 "", flot_val);

  /* big-endian version of the fractional part */
  char* const flot_str_be = str_reverse(flot_str);
  free(flot_str);

  /* combine the flipped fractional part with the integral part */
  snprintf(finalval, (uint16_t) (int_len10 + flot_len10 + /* null + sep */ 2), "%" PRIu64 ".%s", int_val, flot_str_be);

  /* record the integer length */
  *out_int_len = count_digits_u64(int_val);

  return finalval;
}

#endif /* end of include guard: BASE256_H */
