#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>

#define  alloc(size, type) ((type*) malloc(sizeof(type) * size))
#define zalloc(size, type) ((type*) calloc(size, sizeof(type)))

#define FL_SIGN 0x01
#define FL_NAN  0x02
#define FL_INF  0x03

typedef long double ldbl_t;

typedef struct st_bignum_t {
  bool
    sign    : 1,
    is_nan  : 1,
    is_inf  : 1,
    has_cpx : 1,
    has_frc : 1,
    has_exp : 1;

  /*
    value + 0           = number of following integral digits
    value + 1           = number of fractional digits which follow that

    4 3 3 4 5 6 6 7 8 = 3456.678
  */
  uint16_t* value_integ;
          /* optional */
  struct st_bignum_t
          * value_cplex,
          * value_fracl,
          * value_expnt;
} bignum_t;

bignum_t* bignum_ctor (
  const ldbl_t   ldbl,
  const uint64_t u64,
  const bool flags,
  const bignum_t * const * const opt_vals
);
bignum_t* bignum_copy (const bignum_t* const bn, const bool no_recurse_optionals);

uint16_t count_digits_u64 (const uint64_t x);
uint16_t*     to_bn_array (const ldbl_t ldbl, const uint64_t u64);

uint8_t get_left_nth_digit (const uint64_t x, const uint16_t n);

bool compare_eps (const ldbl_t a, const ldbl_t b, const ldbl_t eps);

// i think this is how we compare ie3-754 numbers instead of == in C
bool compare_eps (const ldbl_t a, const ldbl_t b, const ldbl_t eps) {
  return fabsl(a - b) < eps;
}

uint16_t count_digits_u64 (const uint64_t x) {
  return (uint16_t) floor( log10f( (float) x) + 1.f );
}

uint8_t get_left_nth_digit (const uint64_t x, const uint16_t n) {
  return (uint8_t) floor( (double) ((x / ( (uint16_t) pow(10, count_digits_u64(x) - (n + 1)))) % 10) );
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

  const bignum_t st_bn = {
    .sign   = flags & FL_SIGN,
    .is_nan = flags & FL_NAN,
    .is_inf = flags & FL_INF,

    .has_cpx = cx,
    .has_frc = fr,
    .has_exp = ex,

    .value_integ = to_bn_array(ldbl, u64),
    .value_cplex = cx ? bignum_copy(opt_vals[0], true) : NULL,
    .value_fracl = fr ? bignum_copy(opt_vals[1], true) : NULL,
    .value_expnt = ex ? bignum_copy(opt_vals[2], true) : NULL
  };

  bignum_t* hp_bn = alloc(1, bignum_t);

  memcpy(hp_bn, &st_bn, sizeof(st_bn) );

  return hp_bn;
}

bignum_t* bignum_copy (const bignum_t* const bn, const bool no_recurse_optionals) {

  if (NULL == bn) {
    return zalloc(1, bignum_t);
    // return bignum_ctor(0, 0, 0, 0, NULL, NULL, NULL);
  }

  (void) no_recurse_optionals;

  return NULL;
}

uint16_t* to_bn_array (const ldbl_t ldbl, const uint64_t u64) {

  // not zero so we'll use it
  if ( ! compare_eps(ldbl, 0.f, 1e-10) ) {
    return NULL;

  } else if ( 0 != u64 ) {

    #define INIT_LEN 2U
    const uint16_t
      ndigits        = count_digits_u64(u64),
      init[INIT_LEN] = { ndigits, 0 };

    uint16_t* bn_tlated = alloc(ndigits + INIT_LEN + 2, uint16_t);

    memcpy(bn_tlated, &init, INIT_LEN * sizeof(uint16_t));

    for (uint16_t i = 0; i < ndigits; i++) {
      bn_tlated[i + 2] = get_left_nth_digit(u64, i);
    }

    //free(as_str);

    return bn_tlated;

  } else /* both are zero */ {
    return zalloc(2, uint16_t);
  }

  return NULL;
}

