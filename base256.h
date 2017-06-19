#include "bn_common.h"

#define B256_HIGH 256

atom_t* array_reverse (const atom_t* const arr, const uint16_t len) {

  atom_t* result = zalloc(len, atom_t);

  if (len) {
    for (uint16_t i = 0; i < len; i++) {
      result[i] = arr[ i - len ];
    }
  }

  return result;
}

atom_t* u64_digits_to_b256 (const uint64_t value, uint16_t* const len) {
  //atom_t   end_len = ;;
  //atom_t* end_repr = zalloc(size, type)

  if (! value || NULL == len) {
    return zalloc(1, atom_t);
  }

  atom_t* result = zalloc(1, atom_t);
  uint64_t tempr = value;

  uint16_t i = 0;

  /* big endian! */
  for (; tempr; i++) {
    result = realloc(result, i + 1);

    result[i] = (atom_t)   tempr % B256_HIGH;
    tempr     = (uint64_t) floorl(tempr / B256_HIGH);
    //printf("%d, %" PRIu64 "\n", result[i], tempr);
    if (! tempr) { break; }
  }

  *len = i + 1;

  /* big to little endian */
  atom_t* reversed = array_reverse(result, *len);
  free(result);

  return reversed;
}

/* big endian for now */
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

/*atom_t* ldbl_digits_to_b256 (const char* const ldbl_digits, uint16_t* const len, uint16_t* const int_len) {

}
*/
