#ifndef BASE10_H
#define BASE10_H

#include "bn_common.h"

/*
  atom_t*, uint16_t, uint16_t -> char*

  convert a base 10 array into a string base 10 floating point number
  very simple operation from { 1 2 3 4 5 } with int_len = 3 -> "123.45"

  an empty string is returned when
    len is 0
    int_len is greater than len
*/
char* b10_to_ldbl_digits (const atom_t* const digits, const uint16_t len, const uint16_t int_len) {
  if (NULL == digits || ! len || int_len > len) { return make_empty_string(); }

  char* const int_part = alloc(char, len);

  for (size_t i = 0; i < int_len; i++) {
    int_part[i] = (char) ((char) digits[i] + CHAR_DIGIT_DIFF);
  }

  const size_t flot_len = (size_t) ((size_t) len - int_len);

  char* const flot_part = alloc(char, flot_len);

  for (size_t i = 0; i < flot_len; i++) {
    flot_part[i] = (char) ((char) digits[i] + CHAR_DIGIT_DIFF);

  }
  const bool dot = len != int_len;

  char* const final_concat = alloc(char, int_len + dot + flot_len);

  snprintf(final_concat, MAX_STR_LDBL_DIGITS, "%s%s%s", int_part, (dot ? DECIMAL_SEPARATOR_STR : ""), flot_part);

  free(int_part), free(flot_part);

  return final_concat;
}

char* b10_to_u64_digits (const atom_t* const digits, const uint16_t len) {
  char* const u64_str = alloc(char, len + 1);

  for (uint16_t i = 0; i < len; i++) {
    u64_str[i] = (char) (digits[i] + CHAR_DIGIT_DIFF);
  }
  u64_str[len] = '\0';
  return u64_str;
}

uint64_t b10_to_u64 (const atom_t* const digits, const uint16_t len) {
  char* const u64_str = b10_to_u64_digits(digits, len);
  const uint64_t final = strtoull(u64_str, NULL, DEC_BASE);
  free(u64_str);
  return final;
}

// string 123.45 to { 1 2 3 4 5 ... }
atom_t* ldbl_digits_to_b10 (const char* const ldbl_digits, /* out */ uint16_t* const len, /* out */ uint16_t* const int_len, const bool little_endian) {

  if ( (0 == strnlen_c(ldbl_digits, MAX_U64_DIGITS + 2)) || NULL == ldbl_digits || NULL == len || NULL == int_len) {
    set_out_param(len, 1);
    set_out_param(int_len, 1);
    return zalloc(atom_t, 1);
  }

  /* length of integer part before the decimal point */
  const uint16_t pre_dec = (uint16_t) strcspn(ldbl_digits, DECIMAL_SEPARATOR_STR),
              digits_len = (uint16_t) strnlen_c(ldbl_digits, MAX_STR_LDBL_DIGITS);

  char *const int_part  = strndup_c(ldbl_digits, pre_dec), // 1
       *const flot_part = strndup_c(ldbl_digits + pre_dec + 1, MAX_STR_LDBL_DIGITS);

  uint16_t i = 0;
  for (; int_part[i]; i++) {
    int_part[i]  = (char) ((char) int_part[i] - CHAR_DIGIT_DIFF);
  }
  const uint16_t int_part_len = i;

  for (i = 0; flot_part[i]; i++) {
    flot_part[i]  = (char) ((char) flot_part[i] - CHAR_DIGIT_DIFF);
  }
  const uint16_t flot_part_len = i;

  set_out_param(len, (uint16_t) ((uint16_t) int_part_len + flot_part_len));
  set_out_param(int_len, int_part_len);

  atom_t* const res = alloc(atom_t, digits_len);

  memcpy(res, int_part, int_part_len);
  memcpy(res + int_part_len + 1, flot_part, flot_part_len);

  free(int_part), free(flot_part);

  if (little_endian) {
    atom_t* const res_rev = array_reverse(res, (uint16_t)((uint16_t) int_part_len + flot_part_len));
    free(res);
    return res_rev;
  }

  return res;
}

atom_t* u64_to_b10 (const uint64_t value, /* out */ uint16_t* const len, const bool little_endian) {
  char* const str_digits = alloc(char, MAX_U64_DIGITS + 1);
  snprintf(str_digits, MAX_U64_DIGITS, "%" PRIu64, value);

  atom_t* const bytes = u64_digits_to_b10(str_digits, len, little_endian);
  free(str_digits);
  return bytes;
}


// string like 23948734 to atom_t array { 2 3 9 ... }
atom_t* u64_digits_to_b10 (const char* const digits, /* out */ uint16_t* const len, const bool little_endian) {
  const size_t digits_len = strnlen_c(digits, MAX_STR_LDBL_DIGITS);
  if (NULL == digits || 0 == digits_len || NULL == len) {
    return NULL;
  }

  set_out_param(len, (uint16_t) digits_len);
  atom_t* const as_b10 = alloc(atom_t, digits_len);
  for (uint16_t i = 0; i < digits_len; i++) {
    as_b10[i] = (atom_t) ((char) digits[i] - CHAR_DIGIT_DIFF);
  }

  if (little_endian) {
    atom_t* const rev_digits = array_reverse(as_b10, (uint16_t) digits_len);
    free(as_b10);
    return rev_digits;
  }

  return as_b10;
}

#endif
