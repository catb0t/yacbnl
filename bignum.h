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

/*
  bignum_t* -> bignum_t*

  deep copy a bignum_t's properties but not its identity.
*/
bignum_t* bignum_copy (const bignum_t* const bn, const bool no_recurse_optionals) {

  if (NULL == bn) {
    return zalloc(1, bignum_t);
  }

  (void) no_recurse_optionals;

  return NULL;
}

#endif /* end of include guard: BIGNUM_H */
