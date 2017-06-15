#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>

#define HEADER_OFFSET ((atom_t) 3)

#define        sz(n, type) ( ((size_t) n) * (sizeof (type)) )
#define  alloc(size, type) malloc(( (size_t) size) * sizeof (type))
#define zalloc(size, type) calloc(( (size_t) size),  sizeof (type))
#define bna_real_len(bna) ((bna)[0] + (bna)[1] + HEADER_OFFSET)

/* composable */
#define FL_SIGN 0x01
#define FL_NAN  0x02
#define FL_INF  0x04

/* individual values */
typedef enum {
  BN_NONE = 0,

  BN_SIGN = FL_SIGN,
  BN_SNAN = FL_SIGN | FL_NAN,
  BN_SINF = FL_SIGN | FL_INF,

  BN_NAN  = FL_NAN,
  BN_INF  = FL_INF

} bignum_flag_t;

typedef long double ldbl_t;
typedef uint8_t     atom_t;

typedef struct st_bignum_t {

  /*
    value + 0  = number of following integral digits
    value + 1  = number of fractional digits which follow that
    value + 2  = flags, FL_NONE or any OR-combination of FL_INF, FL_NAN and FL_SIGN

    4 3 0 3 4 5 6 6 7 8 = 3456.678
  */
  atom_t
    * value, // the main value
    * imgry, // imaginary part
    * fracl, // fractional part (numerator / denom)
    * vextn; // reference this if bna_real_len(x) >= 255
  struct st_bignum_t* expt;
} bignum_t;

bignum_t* bignum_ctor (const ldbl_t ldbl, const uint64_t u64, const bool flags, const bignum_t * const * const opt_vals);
bignum_t* bignum_copy (const bignum_t* const bn, const bool no_recurse_optionals);

atom_t     count_digits_u64 (const uint64_t x);
atom_t   get_left_nth_digit (const uint64_t x, const atom_t n);
atom_t indexable_digits_u64 (const uint64_t x);

atom_t*         to_bn_array (const ldbl_t ldbl, const uint64_t u64, const atom_t flags);
atom_t* to_bn_array_strimpl (const ldbl_t ldbl, const uint64_t u64, const atom_t flags);

bool compare_eps (const ldbl_t a, const ldbl_t b, const ldbl_t eps);

// i think this is how we compare ie3-754 numbers instead of == in C
bool compare_eps (const ldbl_t a, const ldbl_t b, const ldbl_t eps) {
  return fabsl(a - b) < eps;
}

atom_t count_digits_u64 (const uint64_t x) {
  return (atom_t) floor( log10f( (float) x) + 1.f );
}

atom_t indexable_digits_u64 (const uint64_t x) {
  return (atom_t) floor( log10f( (float) x) );
}

atom_t get_left_nth_digit (const uint64_t x, const atom_t n) {
  const long double
    tpow  = pow(10, indexable_digits_u64(x) - n),
    ldivr = ((long double) x) / tpow;

  //printf("%LG %LG\n", tpow, ldivr);
  return (atom_t) (((uint64_t) ldivr) % 10);
}


bignum_t* bignum_ctor (
  const ldbl_t   ldbl,
  const uint64_t u64,
  const bool flags,
  const bignum_t * const * const opt_vals
) {

  bool cx, fr, ex;

  if (NULL != opt_vals) {
    cx = NULL != opt_vals[0];
    fr = NULL != opt_vals[1];
    ex = NULL != opt_vals[2];
  } else {
    cx = fr = ex = 0;
  }

  bignum_t st_bn = {
    .value = to_bn_array(ldbl, u64, flags),
    .imgry = cx
      ? memcpy(
          alloc( bna_real_len(opt_vals[0]->value), atom_t),
          opt_vals[0]->value,
          sz( bna_real_len(opt_vals[0]->value), atom_t)
        )
      : zalloc(HEADER_OFFSET, atom_t),

    .fracl = fr
      ? memcpy(
        alloc( bna_real_len(opt_vals[1]->value), atom_t),
        opt_vals[1]->value,
        sz( bna_real_len(opt_vals[1]->value), atom_t)
      )
      : zalloc(HEADER_OFFSET, atom_t),

    .expt = ex
      ? bignum_copy(opt_vals[2], true)
      : bignum_ctor(0, 0, 0, NULL)
  };

  bignum_t* hp_bn = memcpy(alloc(1, bignum_t), &st_bn, sizeof(bignum_t) );

  return hp_bn;
}

bignum_t* bignum_copy (const bignum_t* const bn, const bool no_recurse_optionals) {

  if (NULL == bn) {
    return zalloc(1, bignum_t);
  }

  (void) no_recurse_optionals;

  return NULL;
}

#define bna_from_uint(num, flags)  (to_bn_array(0.f, (uint64_t) num, flags))
#define bna_from_flot(flot, flags) (to_bn_array((ldbl_t) flot, 0, flags))
atom_t* to_bn_array (const ldbl_t ldbl, const uint64_t u64, const atom_t flags) {

  // not zero so we'll use it
  if ( ! compare_eps(ldbl, 0.f, 1e-10) ) {
    return NULL;

  // use this instead
  } else if ( 0 != u64 ) {

    const atom_t
      ndigits             = count_digits_u64(u64),
      init[HEADER_OFFSET] = { ndigits, 0, flags };

    atom_t* bn_tlated = alloc(ndigits + HEADER_OFFSET, atom_t);

    memcpy(bn_tlated, &init, sz(HEADER_OFFSET, atom_t));

    for (atom_t i = 0; i < ndigits; i++) {
      //printf("%d %" PRIu64 "\n", i + HEADER_OFFSET, u64);
      bn_tlated[i + HEADER_OFFSET] = get_left_nth_digit(u64, i);
    }

    return bn_tlated;

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

atom_t* to_bn_array_strimpl (const ldbl_t ldbl, const uint64_t u64, const atom_t flags) {
  (void)(ldbl + u64 + flags);
  return NULL;
}
