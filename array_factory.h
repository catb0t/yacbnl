#ifndef BNA_H
#define BNA_H

#include "bn_common.h"
#include "misc_util.h"
#include "addr_interp.h"
#include "base256.h"

static atom_t* impl_to_digit_array_ldbl (const ldbl_t ldbl,  const atom_t metadata, const atom_t flags);
static atom_t*  impl_to_digit_array_u64 (const uint64_t u64, const atom_t metadata, const atom_t flags);

/* create the first 4 or 6 bytes at the beginning of every digit array */
static atom_t* make_array_header (const atom_t metadata, const uint16_t int_digits, const uint16_t flot_digits, const atom_t flags) {

  const atom_t  hdrlen = meta_header_offset(metadata);
  atom_t* const header = zalloc(atom_t, hdrlen);

  /* first byte is the type and base */
  header[0]          = metadata;
  /* last byte is flags */
  header[hdrlen - 1] = flags;

  if ( meta_is_big(metadata) ) {
    atom_t lens[] = { 0, 0, 0, 0 };

    /* set the lengths by using the addresses of the array elements */
    samb_u16_to_twoba(int_digits,  lens + 0, lens + 1);
    samb_u16_to_twoba(flot_digits, lens + 2, lens + 3);

    /* paste four bytes between the metadata and flags */
    memcpy(header + 1, &lens, sz(atom_t, 4) );

  } else {
    header[1] = (atom_t) int_digits;
    header[2] = (atom_t) flot_digits;
  }

  return header;
}

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
  const ldbl_t ldbl  = ldbl_in < 0 ? fabsl(ldbl_in) : ldbl_in;

  // not zero so we'll use it
  if ( ! compare_eps(ldbl, 0.f, 1e-11) ) {

    return impl_to_digit_array_ldbl(ldbl, metadata, flags);

  // use this instead
  } else if ( 0 != u64 ) {

    return impl_to_digit_array_u64(u64, metadata, flags);

  } else /* both are zero, just give zero */ {
    return make_array_header(metadata, 0, 0, flags);
  }

  return NULL;
}

/* IMPLEMENTATIONS */

/*
  ldbl_t, atom_t, atom_t -> atom_t*

  represent a long double number as an array of digits

  the values of metadata and flags are copied into the resulting array

  the caller should have checked whether ldbl is within 1e-11 of 0, therefore,
    this check is not done
*/
static atom_t* impl_to_digit_array_ldbl (const ldbl_t ldbl, const atom_t metadata, const atom_t flags) {

  /* important metadata flags */
  const bool is_base256 = meta_is_base256(metadata),
                 is_big = meta_is_big(metadata);

  /* length of the entire header section */
  const atom_t hdrlen    = meta_header_offset(metadata);
  /* upper bound on significant figures we can store in one array */
  const uint32_t sigfigs = is_big ? MAX_SIGFIGS_BIG : MAX_SIGFIGS;

  /* put the entire value into a string, which may have trailing zeroes */
  char * const fullstr  = alloc(char, sigfigs + 3 /* separator + null */); // 1
  snprintf(fullstr, sigfigs + 2 /* sep */, "%LG", ldbl);

  const atom_t
    // already in fullstr form but the question is, which has lower time complexity
#ifdef PREFER_CHAR_CONV
    // naive string ops
    nint_digits  = (atom_t) strcspn(fullstr, "."),
#else /* ! PREFER_CHAR_CONV */
    // O(1) maths
    nint_digits  = count_digits_u64( (uint64_t) floorl(ldbl) ),
#endif /* PREFER_CHAR_CONV */
    nflot_digits = count_frac_digits(fullstr);

  //printf("digits: %d %d\n", nint_digits, nflot_digits);
  //printf("!!! DBGPRN %LG %s\n", ldbl, fullstr);

  /* make space for the entire new data and store the metadata as a header */
  atom_t *  bn_tlated = alloc(atom_t, nint_digits + nflot_digits + hdrlen),  // 2
         * const init = make_array_header(metadata, nint_digits, nflot_digits, flags); // 3

  /* put the new header in the initial section of new data */
  memcpy(bn_tlated, init, sz(atom_t, hdrlen));
  free(init); // ~3

  if (is_base256) {
    /* convert fullstr to array representation */
    uint16_t len, int_len;
    /* little endian*/
    atom_t* as_digits = ldbl_digits_to_b256(fullstr, &len, &int_len, true);

    free(fullstr);
    /* just copy the data into the rest of the array */
    memcpy(bn_tlated + hdrlen, as_digits, len);

  } else /* base 10 */ {

      /* going to do the integral component */
#ifdef PREFER_CHAR_CONV
    /* more naive simple string operations */
    char* const integ_str = strndup(fullstr, nint_digits); // 4
    for (atom_t i = 0; i < nint_digits; i++) {
      bn_tlated[i + hdrlen] = (atom_t) ((unsigned) integ_str[i] - '0');
    }
    free(integ_str); // ~4
#else /* ! PREFER_CHAR_CONV */
    /* because it is integral we can do this part with integer math */
    for (atom_t i = 0; i < nint_digits; i++) {
      bn_tlated[i + hdrlen] = get_left_nth_digit( (uint64_t) floorl(ldbl), i);
    }
#endif /* PREFER_CHAR_CONV */

    /*
      going to do the fractional component
      could be done by multiplying the fractional part into integrality
      -- but that would not be worth the complexity, because it would require
      you to know the answer (the number of significant digits) ahead of time
    */
    const char* const frac_str = fullstr + find_frac_beginning(fullstr);

    for (size_t i = 0; i < nflot_digits; i++) {
      bn_tlated[i + hdrlen + nint_digits] = (atom_t) ((unsigned) frac_str[i] - '0');
    }

  }
  free(fullstr); //  ~1
  return bn_tlated; // 2
} /* impl_to_digit_array_ldbl */

/*
  uint64_t, atom_t, atom_t -> atom_t*

  represent a uint64_t value as an array of digits

  the values of metadata and flags are copied into the resulting array

  the caller should have checked whether ldbl is 0, therefore, this check is
    not done
*/
static atom_t* impl_to_digit_array_u64 (const uint64_t u64, const atom_t metadata, const atom_t flags) {

  /*
    TODO: add BIG support (?)
  */
  const bool using_base256 = meta_is_base256(metadata);
                    //is_big = meta_is_big(metadata);

  /* number of digits we'll need and the length of the header we'll need */
  const atom_t ndigits = using_base256 ?
                            count_b256_digits_u64(u64) :
                            count_digits_u64(u64),
                hdrlen = meta_header_offset(metadata);

  /* now we can allocate the right size and make a header */
  atom_t* bn_tlated = alloc(atom_t, ndigits + hdrlen), // 1
       * const init = make_array_header(metadata, ndigits, 0, flags); // 2

  /* put the new header in the initial section of new data */
  memcpy(bn_tlated, init, sz(atom_t, hdrlen));
  free(init); // ~2

  /* going to use base 256 */
  if (using_base256) {
    /* convert to array representation */
    uint16_t len;
    /* little endian*/
    atom_t* as_digits = u64_digits_to_b256(u64, &len, true);

    /* just copy the data into the rest of the array */
    memcpy(bn_tlated + hdrlen, as_digits, len);

    free(as_digits);
  } else {

#ifdef PREFER_CHAR_CONV

    /* here begins the string implementation */
    char* const str = alloc( char, ndigits + /* null term */ 2); // 4
    snprintf(str, 21, "%" PRIu64 "", u64);

    for (atom_t i = 0; i < ndigits; i++) {
      bn_tlated[i + hdrlen] = (atom_t) ((unsigned) str[i] - '0');
    }
    free(str); // ~4
    /* here ends the string implementation */

#else /* ! PREFER_CHAR_CONV (default) */

    for (atom_t i = 0; i < ndigits; i++) {
      //printf("%d %" PRIu64 "\n", i + HEADER_OFFSET, u64);
      bn_tlated[i + hdrlen] = get_left_nth_digit(u64, i);
    }

#endif /* PREFER_CHAR_CONV */

  }

  return bn_tlated; // 1
} /* impl_to_digit_array_u64 */

#endif /* end of include guard: BNA_H */

