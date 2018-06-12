#ifndef BASE256_H
#define BASE256_H

#include "bn_common.h"

static uint16_t count_b256_digits_b10 (const char* const digits) {
  (void) digits;
  return 0;
}

/*
  char*, uint16_t*, bool -> atom_t*

  transform a uint64_t (string) to its base 256 representation in bytes

  the value at len is changed to the new length of the representation

  if little_endian is true, then the result is reversed

  a valid pointer to a zero array is returned if
    len is NULL
    the input string is empty or NULL

  in the last case, len is set to 1 (when a valid pointer)
*/
atom_t* u64_digits_to_b256 (const char* const u64_str, uint16_t* const len, const bool little_endian) {

  if (string_is_sempty(u64_str, MAX_STR_LDBL_DIGITS) || NULL == len) {
    set_out_param(len, 1);
    return zalloc(atom_t, 1);
  }

  atom_t* const result = zalloc(atom_t, count_b256_digits_b10(u64_str));

  atom_t* const as_b10 = u64_digits_to_b10(u64_str, len, false);
  (void) little_endian;
  (void) as_b10;

  /* big endian! */
  /*
    for (uint64_t tempr = value; tempr; i++) {
      //result = realloc(result, i + 1);

      result[i] = (atom_t)   (tempr % B256_HIGH);
      tempr     = (uint64_t) floorl(tempr / B256_HIGH);
      //printf("%d, %" PRIu64 "\n", result[i], tempr);
      if (! tempr) { break; }
    }
  */

  return result;
}

/*
  char* -> atom_t*
*/
static atom_t* str_digits_to_b256 (const char* const digits, uint16_t* const len, const bool little_endian) {
  (void) digits;
  (void) len;
  (void) little_endian;
  return NULL;
}
/*
{
  const char* const rev_digits = str_reverse(digits);


  size_t b256_place = 1, b10_place = 1; // 1-index

  atom_t* const b256_digits = alloc(atom_t, 10);

  for (size_t i = 0; rev_digits[i]; i++) {
  }

  return b256_digits;
}
*/
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

  if ( string_is_sempty(ldbl_digits, MAX_STR_LDBL_DIGITS) || NULL == len || NULL == int_len) {
    set_out_param(len, 1);
    set_out_param(len, 1);
    return zalloc(atom_t, 1);
  }

  /* length of integer part before the decimal point */
  const uint16_t pre_dec = (uint16_t) strcspn(ldbl_digits, DECIMAL_SEPARATOR_STR);

  /* get the two parts of the number */
  char* const int_part  = strndup_c(ldbl_digits, pre_dec), // 1
      /* flip the significant digits */
      * const flot_part = str_reverse(ldbl_digits + pre_dec + /* skip separator */ 1); // 2

  uint16_t lhs_len = 0, rhs_len = 0;
  atom_t* const lhs_b256 = str_digits_to_b256(int_part, &lhs_len, true),
        /* note argument #3: flip the fractional digits again */
        * const rhs_b256 = str_digits_to_b256(flot_part, &rhs_len, false);

  set_out_param(len, (uint16_t) (lhs_len + rhs_len) );
  set_out_param(int_len, lhs_len);

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

  the following functions ONLY expect and return data with the least significant part
    at the end, rather than at the beginning

  this is in line with how numbers are usually written in both base 10 and base 2
*/

/*
  atom_t*, uint16_t -> uint64_t

  get the base 10 value out of a base 256 representation

  this function clears errno on entry, but not on exit

  0 is returned when
    digits represents the value 0
    len is 0 or greater than MAX_U64_DIGITS (usually 20)
    the value represented in digits is larger than UINT64_MAX

  in the last two cases, errno is set to ERANGE
*/
uint64_t b256_to_u64 (const atom_t* const digits, const uint16_t len) {

  if (! digits || ! len ) {
    return 0;
  }

  if (len > MAX_U64_DIGITS) { goto range_err; }

  errno = 0;
  uint64_t result = 0;

  for (int16_t i = (int16_t) (len - 1); i > -1; i--) {
    result += digits[i] * ( (uint64_t) powl(ZENZ_BASE, i) );
    if (ERANGE == errno) { goto range_err; }
  }

  return result;

  range_err:
    errno = ERANGE;
    return 0;
}

char* b256_to_u64_digits (const atom_t* const digits, const uint16_t len) {
  (void) digits;
  (void) len;
  return NULL;
}

/*
  atom_t*, uint16_t, uint16_t, uint16_t* -> char*

  convert base 256 representation of a long double to base 10 string

  an empty string is returned when
    len is 0
    int_len is greater than len
    out_int_len is not writable (i.e. NULL)
    digits is NULL
*/
char* b256_to_ldbl_digits (const atom_t* const digits, const uint16_t len, const uint16_t int_len) {

  /* prologue: check arguments */

  /* 0 is a valid int_len */
  if (NULL == digits || ! len || int_len > len) {
    return make_empty_string();
  }

  /* step 1: base 256 -> base 10 parts */

  /* copy value to a better name */
  const uint16_t int_len_orig = int_len;

  /* the length of the fractional part is from subtraction */
  const uint16_t flot_len_orig = (uint16_t) (len - int_len_orig);

  /* copy the parts (in base 256) from the original (1 2) */
  atom_t *const int_b256  = memcpy( alloc(atom_t, int_len_orig), digits, sz(atom_t, int_len_orig)),
  /* the flot part needs to be reversed so that it is little endian (descending places from left to right) */
         *const flot_b256 = array_reverse(digits + int_len_orig, flot_len_orig); // big endian -> le

  /* step 2: base 10 parts -> strings */

  /* convert / reverse the parts (3 4 5) */
  char *const int_b10     = b256_to_u64_digits(int_b256, int_len_orig),
  /* this conversion will be wrong if the flot part is left big endian */
       *const flot_b10    = b256_to_u64_digits(flot_b256, flot_len_orig), // le -> le
  /* re-reverse the flot part so that it is in the proper order */
       *const flot_b10_be = str_reverse(flot_b10); // le -> be

  /* don't need the base256 parts anymore (~1 ~2 ~4) */
  free(int_b10), free(flot_b256), free(flot_b10);

  /* step 3: strings -> string + sep + string */

  const uint16_t int_len_b10 = (uint16_t) strnlen_c(int_b10, MAX_STR_LDBL_DIGITS),
                flot_len_b10 = (uint16_t) strnlen_c(flot_b10_be, MAX_STR_LDBL_DIGITS);

  // 6
  char* const out_str = alloc(char, 2 + int_len_b10 + flot_len_b10);

  snprintf(out_str, 1U + int_len_b10 + flot_len_b10, "%s.%s", int_b10, flot_b10_be);

  // ~3 ~5
  free(int_b10), free(flot_b10_be);

  /* epilogue: finish */

  // 6 leaves scope
  return out_str;
}

atom_t*          u64_to_b256 (const uint64_t value, uint16_t* const len, const bool little_endian) {
  char* const val_str = alloc(char, count_digits_u64(value));

  snprintf(val_str, MAX_U64_DIGITS, "%" PRIu64 "", value);

  atom_t* const result = u64_digits_to_b256(val_str, len, little_endian);
  free(val_str);

  return result;
}

#endif /* end of include guard: BASE256_H */
