#ifndef BASE256_H
#define BASE256_H

#include "bn_common.h"

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
