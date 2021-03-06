#ifndef MISC_UTIL_H
#define MISC_UTIL_H

#include "bn_common.h"

// i think this is how we compare ie3-754 numbers instead of == in C
bool compare_eps (const ldbl_t a, const ldbl_t b, const ldbl_t eps) {
  return fabsl(a - b) < eps;
}

/*
  uint64_t -> atom_t

  count the number of actual digits in a base 10 number
  the result is always less than 21 because that's all you get with 64 bits
*/
atom_t count_digits_u64 (const uint64_t x) {
  return (atom_t) floor( log10f( (float) x) + 1.f );
}

/*
  atom_t*, uint16_t -> bool
*/
bool raw_is_zero (const atom_t* const digits, const uint16_t len) {
  uint16_t sum = 0;
  for (uint16_t i = 0; i < len; i++) {
    sum = (uint16_t) ((uint16_t) digits[i] + sum);
  }
  return 0 == sum;
}

/*
  char* -> atom_t

  find the beginning of the fractional part of the string representation of a
    floating point numuber
*/
atom_t find_frac_beginning (const char* const str) {
  //begin_read = (atom_t) (1U + (unsigned) pre_len);
  const atom_t
    pre_len = (atom_t) strcspn(str, DECIMAL_SEPARATOR_STR),
    len  = (atom_t) strnlen_c(str, MAX_PRIMITIVE_LDBL_DIGITS),
    diff = (atom_t) (len - pre_len);
    /* found + 1 for separator */

  if (1 == diff) {
    return pre_len;
  } else if (diff) {
    return (atom_t) (1 + pre_len);
  } else {
    return len;
  }

}

/*
  char* -> atom_t

  count the number of actual digits in a string representation of a floating
    point number

  numbers like 123. end with an implicit zero and have a fractional length of 1
*/
atom_t count_frac_digits (const char* const str) {
  const atom_t begin = find_frac_beginning(str);
  if (1 == strnlen_c(str, MAX_PRIMITIVE_LDBL_DIGITS) - begin) { return 1; }
  return (atom_t) strspn(str + begin, "0123456789");
}

/*
  uint64_t -> atom_t

  report the highest indexable digit of a number
  0 based counting; constrast with count_digits_u64
  for use with get_left_nth_digit
*/
atom_t indexable_digits_u64 (const uint64_t x) {
  return (atom_t) floor( log10f( (float) x) );
}

/*
  uint64_t, atom_t -> atom_t

  report the value in a given 10s place of a number
  indexing is 0-based and from the left side (greatest significand)

  uses a fast ( theoretically O(1) ) number-theory formula by default

  define PREFER_CHAR_CONV and you get a much slower string-based implementation
    that relies much less on floating point math
    -- it's what you asked for
*/
atom_t get_left_nth_digit (const uint64_t x, const atom_t n) {
#ifdef PREFER_CHAR_CONV

  char* const str = alloc(char, MAX_U64_DIGITS + 2);
  snprintf(str, MAX_U64_DIGITS + 1, "%" PRIu64 "", x);

  const char d = str[n];
  free(str);
  return (atom_t) ((unsigned) d - '0');

#else /* ! PREFER_CHAR_CONV */

  const ldbl_t tpow  = pow(DEC_BASE, indexable_digits_u64(x) - n),
               ldivr = ((ldbl_t) x) / tpow;

  //printf("%LG %LG\n", tpow, ldivr);
  return (atom_t) (((uint64_t) ldivr) % DEC_BASE);

#endif /* PREFER_CHAR_CONV */
}

/*
  char* -> char*

  reverse a string.
  the return value will never point to the input value, hence const* const

  the return value may be NULL but will not be an otherwise invalid pointer
*/
char* str_reverse (const char* const str) {
  if ( NULL == str ) { return NULL; }

  size_t len = strnlen_c(str, MAX_EXPORT_SIGFIGS);
  char* newp = NULL;

  if ( len < 2 ) {
    newp = strndup_c(str, len);

  } else {
    newp = alloc(char, len + 1);

    size_t i, j;
    for (i = 0, j = len - 1; i < len; i++, j--) {
      newp[i] = str[j];
      //printf("i: %zu, j: %zu, ic: %c, ij: %c\n", i, j, newp[i], str[j]);
    }

    newp[i] = '\0';
  }

  return newp;
}


/*
  float -> float

  simple base 256 logarithm
*/
float log256f (const float x) {
  return logf(x) / logf(ZENZ_BASE);
}

/*
  uint64_t -> atom_t

  count the number of digits needed in base 256 to represent x
*/
atom_t count_b256_digits_u64 (const uint64_t x) {
  return (atom_t) floorf( log256f( (float) x ) + 1.f);
}

/*
  char* -> uint16_t

  count the number of digits needed in base 256 to represent the base 10 input
*/
uint16_t count_b256_digits_b10_digits (const char* const digits) {
  puts("UNIMPLEMENTED");
  uint16_t len_initial = 0;
  atom_t* const as_atoms = u64_digits_to_b10(digits, &len_initial, false);
  static const atom_t b10_256[3] = { 2, 5, 6 };
  // logn_b10(256, digits)
  uint16_t log_len = 0, log_int_len = 0;
  atom_t* const log256 = logn_b10 (b10_256, 3, 3, as_atoms, len_initial, len_initial, &log_len, &log_int_len);
  free(as_atoms);
  // add 1
  uint16_t len_add1 = 0, int_len_add1 = 0;
  atom_t* const add1 = succ_b10(log256, len_initial, len_initial, 0, &len_add1, &int_len_add1);
  free(log256);
  // floor
  uint16_t floored_len = 0, floored_int_len = 0;
  atom_t* const floored = floor_b10(add1, len_add1, int_len_add1, &floored_len, &floored_int_len);
  free(add1);
  // convert to hardware
  const uint16_t final = b10_to_u16(floored, len_add1);
  free(floored);
  return final;
}

/*
  atom_t*, uint16_t -> atom_t*

  copy and reverse an array
  the return value will always be a valid unique pointer
*/
atom_t* array_reverse (const atom_t* const arr, const uint16_t len) {

  atom_t* result = alloc(atom_t, len);

  if (len) {
    for (uint16_t i = 0; i < len; i++) {
      result[i] = arr[ (len - 1) - i ];
    }
  }

  return result;
}

/*
  atom_t*, atom_t*, uint16_t, uint16_t -> atom_t*

  glue two arrays together

  always returns valid unique pointers
*/
atom_t* array_concat (const atom_t* const a, const uint16_t a_len, const atom_t* const b, const uint16_t b_len) {
  if (! (a_len + b_len) || NULL == a || NULL == b) {
    return alloc(atom_t, 0);
  }

  // b is empty ; b is not
  else if (a_len == (a_len + b_len)) {
    return (atom_t*) memcpy(alloc(atom_t, a_len), a, a_len);

  } else if (b_len == a_len + b_len) {
    return (atom_t*) memcpy(alloc(atom_t, b_len), b, b_len);
  }

  // copy one array
  atom_t* const res = (atom_t*) memcpy( alloc(atom_t, a_len + b_len) , a, a_len);
  // and the second one
  memcpy( res + a_len, b, b_len);
  // and then return res, not res + a_len
  return              (atom_t*) res;
}

bool array_contains (const atom_t* const arr, const uint16_t len, const atom_t value) {
  if (len) {
      for (size_t i = 0; i < len; i++) {
      if (value == arr[i]) { return true; }
    }
  }
  return false;
}

atom_t* array_copy (const atom_t* const a, const uint16_t len) {
  return (atom_t*) memcpy(alloc(atom_t, len),a, len);
}

/*
  -> char*

  return an empty (0-length) string
*/
char* make_empty_string (void) {
  return zalloc(char, 1);
}

/*
  atom_t*, uint16_t, uint16_t, atom_t... -> uint16_t

  strspn / strcspn but for arrays
  return the length of the initial section of arr which consists only of values
    in accept_only
  see strspn(3) and strcspn(3)
*/
uint16_t array_span (const atom_t* arr, const uint16_t arr_len, const bool accept, const atom_t* const vals, const uint16_t vals_len) {
  uint16_t i = 0;
  for (; i < arr_len; i++) {
    if (accept == !array_contains(vals, vals_len, arr[i])) {
      break;
    }
  }
  return i;
}

/*
  atom_t* -> atom_t*

  remove insignificant trailing zeroes from an array of digits in any base
    since the representation of literal 0 is constant across radicies
*/
atom_t* array_trim_trailing_zeroes (const atom_t* const bn) {
  const atom_t hdrlen = bna_header_offset(bn);
  const bool   is_big = bna_is_big(bn);

  const uint16_t
    //len     = (uint16_t) bna_real_len(bn),
    int_len = bna_int_len(bn),
    frc_len = bna_frac_len(bn);

  //atom_t const * rev_cpy = array_reverse(bn, len);

  //uint16_t consec_zeroes = array_spn(rev_cpy, len, 1, 0);
  puts("UNIMPLEMENTED");
  (void) hdrlen;
  (void) is_big;
  (void) int_len;
  (void) frc_len;
  //(void) consec_zeroes;

  return NULL;
}

/*
  atom_t* -> atom_t*

  remove insignificant leading zeroes from an array of digits in any base
*/
atom_t* array_trim_leading_zeroes_simple (const atom_t* const bn, const uint16_t len, uint16_t* const out_len) {
  atom_t* const z = zalloc(atom_t, 1);
  const uint16_t count_leading_zeroes = array_span(bn, len, true, z, 1);
  free(z);
  const uint16_t nonzeroes = (uint16_t) (len - count_leading_zeroes);
  set_out_param(out_len, nonzeroes);
  return (atom_t*) memcpy(alloc(atom_t, nonzeroes), bn + count_leading_zeroes, nonzeroes);
}

char* strndup_c (const char* const s, size_t const n) {
  const size_t len = strnlen_c(s, n);
  char* const news = alloc(char, len + 1);

  if (NULL == news) {
    return NULL;
  }

  news[len] = '\0';
  return (char*) memcpy(news, s, len);
}

size_t strnlen_c (const char* const s, const size_t maxsize) {
  if (NULL == s) {
      return 0;
  }

  size_t i = 0;
  for (; i < maxsize; i++) {
    if ('\0' == s[i]) { return i; }
  }

  return i;
}

size_t str_count (const char* const str, const char find) {
  size_t ocur = 0;
  const size_t len = strnlen_c(str, MAX_STR_LDBL_DIGITS);
  for (size_t i = 0; i < len; i++) {
    if (find == str[i]) { ++ocur; }
  }
  return ocur;
}

void _say_atom_t_ptr (const atom_t* const data, const uint16_t len) {
  printf("atom_t ptr %p+%d: ", (const void* const) data, len);
  for (uint16_t i = 0; i < len; i++) {
    printf("%d ", data[i]);
  }
  printf(";\n");
}

#endif /* end of include guard: MISC_UTIL_H */
