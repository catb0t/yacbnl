#ifndef BNA_H
#define BNA_H

#include "bn_common.h"

static atom_t* impl_to_bn_array_ldbl (const ldbl_t ldbl, const atom_t flags);
static atom_t*  impl_to_bn_array_u64 (const uint64_t u64, const atom_t flags);

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

  char* const str = alloc(21, char);
  snprintf(str, 20, "%" PRIu64 "", x);

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

/*
  ldbl_t, uint64_t, atom_t -> atom_t*

  make a new bignum array "underlying" object from the first nonzero of two
  primitive arguments, or zero if both are.

  "metadata", like signedness, infiniteness and nan-ness should be indicated in
  flags, not some cryptic value for argument #1 ldbl_t

  for example, signed zero would best be represented by to_bn_array(0, 0, FL_SIGN).
*/
#define bna_from_uint(num, flags)  (to_bn_array(0.f, (uint64_t) num, flags))
#define bna_from_flot(flot, flags) (to_bn_array((ldbl_t) flot, 0, flags))
atom_t* to_bn_array (const ldbl_t ldbl_in, const uint64_t u64, const atom_t flags_in) {

  const atom_t flags = ldbl_in < 0 ? flags_in | FL_SIGN : flags_in;
  const ldbl_t ldbl  = ldbl_in < 0 ? fabsl(ldbl_in) : ldbl_in;

  // not zero so we'll use it
  if ( ! compare_eps(ldbl, 0.f, 1e-11) ) {

    return impl_to_bn_array_ldbl(ldbl, flags);

  // use this instead
  } else if ( 0 != u64 ) {

    return impl_to_bn_array_u64(u64, flags);

  } else /* both are zero, just give zero */ {
    const atom_t zero[] = { 0, 0, flags };

    return memcpy(
      alloc(HEADER_OFFSET, atom_t),
      &zero,
      sz(HEADER_OFFSET, atom_t)
    );
  }

  return NULL;
}

static atom_t* impl_to_bn_array_ldbl (const ldbl_t ldbl, const atom_t flags) {
  // 504 = (255 * 2) - (HEADER_OFFSET * 2) usually but it probably won't all be used
  char* const fullstr = alloc(MAX_SIGFIGS + 3 /* separator + null */, char);

  /* IEEE 754 is a weird beast but really, if you
    - can get >252 sigfigs from 80 bits (8087 / intel long double) or
    - have a hardware long double that can preserve >252 sigfigs
    then why are you running this code anyways
  */
  snprintf(fullstr, MAX_SIGFIGS + 1 /* sep */, "%.252LG", ldbl);

  const atom_t
    // already in fullstr form but the question is, which has lower time complexity
#ifdef PREFER_CHAR_CONV
    nint_digits         = (atom_t) strcspn(fullstr, "."),
#else /* ! PREFER_CHAR_CONV */
    nint_digits         = count_digits_u64( (uint64_t) floorl(ldbl) ),
#endif /* PREFER_CHAR_CONV */
    nflot_digits        = count_frac_digits(fullstr),
    init[HEADER_OFFSET] = { nint_digits, nflot_digits, flags };

  atom_t* bn_tlated = alloc(nint_digits + nflot_digits + HEADER_OFFSET, atom_t);

  memcpy(bn_tlated, &init, sz(HEADER_OFFSET, atom_t));

  /* going to do the integral component */
#ifdef PREFER_CHAR_CONV
  char* const integ_str = strndup(fullstr, nint_digits);
  for (atom_t i = 0; i < nint_digits; i++) {
    bn_tlated[i + HEADER_OFFSET] = (atom_t) ((unsigned) integ_str[i] - '0');
  }
  free(integ_str);
#else /* ! PREFER_CHAR_CONV */
  for (atom_t i = 0; i < nint_digits; i++) {
    bn_tlated[i + HEADER_OFFSET] = get_left_nth_digit( (uint64_t) floorl(ldbl), i);
  }
#endif /* PREFER_CHAR_CONV */

  /* going to do the fractional component */
  // must be done with string

  const char* const frac_str = fullstr + find_frac_beginning(fullstr);

  for (size_t i = 0; i < nflot_digits; i++) {
    bn_tlated[i + HEADER_OFFSET + nint_digits] = (atom_t) ((unsigned) frac_str[i] - '0');
  }

  free(fullstr);
  return bn_tlated;
}

static atom_t* impl_to_bn_array_u64 (const uint64_t u64, const atom_t flags) {
  const atom_t ndigits             = count_digits_u64(u64),
               init[HEADER_OFFSET] = { ndigits, 0, flags };

  atom_t* bn_tlated = alloc(ndigits + HEADER_OFFSET, atom_t);

  memcpy(bn_tlated, &init, sz(HEADER_OFFSET, atom_t));

#ifdef PREFER_CHAR_CONV

  /* here begins the string implementation */
  char* const str = alloc( ndigits + /* null term */ 2, char);
  snprintf(str, 21, "%" PRIu64 "", u64);

  for (atom_t i = 0; i < ndigits; i++) {
    bn_tlated[i + HEADER_OFFSET] = (atom_t) ((unsigned) str[i] - '0');
  }
  free(str);
  /* here ends the string implementation */

#else /* ! PREFER_CHAR_CONV (default) */

  for (atom_t i = 0; i < ndigits; i++) {
    //printf("%d %" PRIu64 "\n", i + HEADER_OFFSET, u64);
    bn_tlated[i + HEADER_OFFSET] = get_left_nth_digit(u64, i);
  }

#endif /* PREFER_CHAR_CONV */

  return bn_tlated;
}

#endif /* end of include guard: BNA_H */
