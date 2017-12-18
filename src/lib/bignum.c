#ifndef BIGNUM_H
#define BIGNUM_H

#include "bn_common.h"

/*
  ldbl_t, uint64_t, atom_t, bignum_t** -> bignum_t*

  create a new bignum out of primitives and values or give zero
*/
bignum_t* bignum_ctor (
  const ldbl_t   ldbl,
  const uint64_t u64,
  const atom_t flags,
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

  /* create a stack-allocated version of the structure */
  bignum_t st_bn = {
    /* real value */
    .value = to_digit_array(ldbl, u64, flags, 0),
    /* imaginary part is determined */
    .imgry = cx
      ? memcpy(
          alloc(atom_t, bna_real_len(opt_vals[0]->value)),
          opt_vals[0]->value,
          sz(atom_t, bna_real_len(opt_vals[0]->value))
        )
      : zalloc(atom_t, HEADER_OFFSET),
    /* fractional (numerator / denominator) */
    .fracl = fr
      ? memcpy(
        alloc(atom_t, bna_real_len(opt_vals[1]->value)),
        opt_vals[1]->value,
        sz(atom_t, bna_real_len(opt_vals[1]->value))
      )
      : zalloc(atom_t, HEADER_OFFSET),
    /* exponent */
    .expt = ex
      ? bignum_copy(opt_vals[2], true)
      : bignum_ctor(0, 0, 0, NULL)
  };

  /* copy it to the heap */
  bignum_t* hp_bn = memcpy( alloc(bignum_t, 1), &st_bn, sizeof(bignum_t) );

  return hp_bn;
}

/*
  bignum_t* -> bignum_t*

  deep copy a bignum_t's properties but not its identity
*/
bignum_t* bignum_copy (const bignum_t* const bn, const bool no_recurse_optionals) {

  if (NULL == bn) {
    return zalloc(bignum_t, 1);
  }

  (void) no_recurse_optionals;

  return NULL;
}

#endif /* end of include guard: BIGNUM_H */
