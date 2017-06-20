#include "bn_common.h"

atom_t* u64_digits_to_b256 (const uint64_t value, uint16_t* const len, const bool little_endian) {

  if (! value || NULL == len) {
    return zalloc(1, atom_t);
  }

  atom_t* result = zalloc(count_b256_digits_u64(value), atom_t);
  uint64_t tempr = value;

  uint16_t i = 0;

  /* big endian! */
  for (; tempr; i++) {
    //result = realloc(result, i + 1);

    result[i] = (atom_t)   tempr % B256_HIGH;
    tempr     = (uint64_t) floorl(tempr / B256_HIGH);
    //printf("%d, %" PRIu64 "\n", result[i], tempr);
    if (! tempr) { break; }
  }

  *len = i + 1;

  if (little_endian) {
    atom_t* reversed = array_reverse(result, *len);
    free(result);

    return reversed;
  } else {
    return result;
  }
}

/* little endian */
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

atom_t* ldbl_digits_to_b256 (const char* const ldbl_digits, uint16_t* const len, uint16_t* const int_len) {

  uint16_t pre_dec = (uint16_t) strcspn(ldbl_digits, ".");

  char* const int_part  = strndup(ldbl_digits, pre_dec),
      /* flip the significant digits */
      * const flot_part = str_reverse(ldbl_digits + pre_dec + 1); /* skip separator */

  /* convert the strings to numbers */
  const uint64_t lhs = strtoull(int_part,  NULL, B10_HIGH),
                 rhs = strtoull(flot_part, NULL, B10_HIGH);

  uint16_t lhs_len, rhs_len;
  atom_t* const lhs_b256 = u64_digits_to_b256(lhs, &lhs_len, true),
        /* flip the fractional digits again */
        * const rhs_b256 = u64_digits_to_b256(rhs, &rhs_len, false);

  *len     = lhs_len + rhs_len;
  *int_len = lhs_len;

  atom_t* final_concat = array_concat(lhs_b256, rhs_b256, lhs_len, rhs_len);

  free(lhs_b256), free(rhs_b256);

  return final_concat;
}

