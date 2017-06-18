#include "bn_common.h"

// i think this is how we compare ie3-754 numbers instead of == in C
bool compare_eps (const ldbl_t a, const ldbl_t b, const ldbl_t eps) {
  return fabsl(a - b) < eps;
}

/*
  uint64_t -> atom_t

  count the number of actual digits in a base 10 number.
  the result is always less than 21 because that's all you get with 64 bits
*/
atom_t count_digits_u64 (const uint64_t x) {
  return (atom_t) floor( log10f( (float) x) + 1.f );
}

/*
  char* -> atom_t

  find the beginning of the fractional part of the string representation of a
  floating point numuber
*/
atom_t find_frac_beginning (const char* const str) {
  const atom_t pre_len = (atom_t) strcspn(str, ".");
            //begin_read = (atom_t) (1U + (unsigned) pre_len);
            /* + 1 for separator */
  return (atom_t) (1U + (unsigned) pre_len);
}

/*
  char* -> atom_t

  count the number of actual digits in a string representation of a floating
  point number
*/
atom_t count_frac_digits (const char* const str) {
  return (atom_t) strspn(str + find_frac_beginning(str), "0123456789");
}

/*
  uint64_t -> atom_t

  report the highest indexable digit of a number; for use with get_left_nth_digit.
  0 based counting; constrast with count_digits_u64.
*/
atom_t indexable_digits_u64 (const uint64_t x) {
  return (atom_t) floor( log10f( (float) x) );
}

/*
  uint64_t, atom_t -> atom_t

  report the value in a given 10s place of a number
  indexing is 0-based and from the left side (greatest significand).

  uses a fast ( theoretically O(1) ) number-theory formula by default.

  define PREFER_CHAR_CONV and you get a much slower, string-based implementation,
  that relies much less on floating point math.
  it's what you asked for.
*/
atom_t get_left_nth_digit (const uint64_t x, const atom_t n) {
#ifdef PREFER_CHAR_CONV

  char* const str = alloc(22, char);
  snprintf(str, 21, "%" PRIu64 "", x);

  char d = str[n];
  free(str);
  return (atom_t) ((unsigned) d - '0');

#else /* ! PREFER_CHAR_CONV */

  const ldbl_t tpow  = pow(10, indexable_digits_u64(x) - n),
               ldivr = ((ldbl_t) x) / tpow;

  //printf("%LG %LG\n", tpow, ldivr);
  return (atom_t) (((uint64_t) ldivr) % 10);

#endif /* PREFER_CHAR_CONV */
}
