#ifndef MISC_UTIL_H
#define MISC_UTIL_H

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

char* str_reverse (const char* const str) {
  if ( NULL == str ) { return NULL; }

  size_t len = strnlen(str, MAX_SIGFIGS);
  char* newp;

  if ( len < 2 ) {
    newp = strndup(str, len);

  } else {
    newp = alloc(len + 1, char);

    size_t i, j;
    for (i = 0, j = len - 1; i < len; i++, j--) {
      newp[i] = str[j];
      //printf("i: %zu, j: %zu, ic: %c, ij: %c\n", i, j, newp[i], str[j]);
    }

    newp[i] = '\0';
  }

  return newp;
}

float log256f (const float x) {
  return logf(x) / logf(256);
}

atom_t count_b256_digits_u64 (const uint64_t x) {
  return (atom_t) floorf( log256f( (float) x ) + 1.f);
}

atom_t* array_reverse (const atom_t* const arr, const uint16_t len) {

  atom_t* result = zalloc(len, atom_t);

  if (len) {
    for (uint16_t i = 0; i < len; i++) {
      result[i] = arr[ i - len ];
    }
  }

  return result;
}

atom_t* array_concat (const atom_t* const a, const atom_t* const b, const uint16_t a_len, const uint16_t b_len) {
  if (! (a_len + b_len) ) {
    return alloc(0, atom_t);
  }

  else if (a_len == a_len + b_len) {
    return memcpy(alloc(a_len, atom_t), a, a_len);

  } else if (b_len == a_len + b_len) {
    return memcpy(alloc(b_len, atom_t), b, b_len);
  }

  atom_t* const res = memcpy( alloc(a_len + b_len, atom_t) , a, a_len);
  return              memcpy( res + a_len, b, b_len);
}

#endif /* end of include guard: MISC_UTIL_H */
