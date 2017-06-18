#ifndef BNA_H
#define BNA_H

#include "bn_common.h"
#include "misc_util.h"
#include "addr_interp.h"

static atom_t*    impl_to_digit_array_ldbl (const ldbl_t ldbl,  const atom_t metadata, const atom_t flags);
static atom_t*     impl_to_digit_array_u64 (const uint64_t u64, const atom_t metadata, const atom_t flags);
//static atom_t* impl_to_digit_bigarray_ldbl (const ldbl_t ldbl,  const atom_t flags, const atom_t metadata);
//static atom_t*  impl_to_digit_bigarray_u64 (const uint64_t u64, const atom_t flags, const atom_t metadata);


/*
  ldbl_t, uint64_t, atom_t -> atom_t*

  make a new bignum array "underlying" object from the first nonzero of two
  primitive arguments, or zero if both are.

  value metadata, like signedness, infiniteness and nan-ness should be indicated in
  flags, not some cryptic value for argument #1 ldbl_t

  for example, signed zero would best be represented by to_bn_array(0, 0, FL_SIGN, 0).
*/
atom_t* to_digit_array (const ldbl_t ldbl_in, const uint64_t u64, const atom_t value_flags, const atom_t metadata) {

  const atom_t flags = ldbl_in < 0 ? value_flags | FL_SIGN : value_flags;
  const ldbl_t ldbl   = ldbl_in < 0 ? fabsl(ldbl_in) : ldbl_in;

  // not zero so we'll use it
  if ( ! compare_eps(ldbl, 0.f, 1e-11) ) {

    return impl_to_digit_array_ldbl(ldbl, metadata, flags);

  // use this instead
  } else if ( 0 != u64 ) {

    return impl_to_digit_array_u64(u64, metadata, flags);

  } else /* both are zero, just give zero */ {
    atom_t* const zero = zalloc(metadata & TYP_BIG ? HEADER_OFFSET_BIG : HEADER_OFFSET, atom_t);

    zero[0]                     = metadata;
    zero[HEADER_OFFSET_BIG - 1] = flags;

    return zero;
  }

  return NULL;
}

/* IMPLEMENTATIONS */

static atom_t* impl_to_digit_array_ldbl (const ldbl_t ldbl, const atom_t metadata, const atom_t flags) {

  const bool using_base256 = metadata & TYP_ZENZ;
  const bool using_bigaddr = metadata & TYP_BIG;

  //
  // 504 = (255 * 2) - (HEADER_OFFSET * 2) usually but it probably won't all be used
  char* const fullstr = alloc(MAX_SIGFIGS + 3 /* separator + null */, char);

  /* IEEE 754 is a weird beast but really, if you
    - can get so many sigfigs from 80 bits (8087 / intel long double) or
    - have a hardware long double that can preserve so many sigfigs
    then why are you running this code anyways
  */
  snprintf(
    fullstr,
    MAX_SIGFIGS + 1 /* sep */,
    "%." stringify(PRIMITIVE_PRECISION) "LG",
    ldbl
  );
  printf("%s %zu  \n", fullstr, strnlen(fullstr, PRIMITIVE_PRECISION));

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

static atom_t* impl_to_digit_array_u64 (const uint64_t u64, const atom_t metadata, const atom_t flags) {
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

/* !! BIG !! */


/*static atom_t* impl_to_dec_bigarray_u64 (const uint64_t u64, const atom_t flags) {
  return NULL;
}

static atom_t* impl_to_dec_bigarray_ldbl (const ldbl_t ldbl, const atom_t flags) {
  return NULL;
}
*/
#endif /* end of include guard: BNA_H */

