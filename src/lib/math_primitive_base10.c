#ifndef MATH_PRIMITIVE_BASE10
#define MATH_PRIMITIVE_BASE10

#include "bn_common.h"
/* simple unsigned real number math */


static atom_t* impl_pred_b10_int (const atom_t* const n, const uint16_t len, uint16_t* const out_len) {
  // last digit is not 0
  if (0 != n[len - 1]) {
    set_out_param(out_len, len);
    // only the last digit changes, so copy all the preceding digits
    atom_t* const result = memcpy(alloc(atom_t, len), n, (uint16_t) (len - 1));
    // then take 1
    result[len - 1] = (atom_t) ((atom_t) n[len - 1] - 1);
    return result;
  } else {
    /*
      12300 -> 00321 -> 321 -> 221 -> 99221 -> 12299
      12100 -> 00121 -> 121 -> 021 -> 99021 -> 12099
      50000 -> 00005 -> 5   -> 4   -> 99994 -> 49999
      10000 -> 00001 -> 1   -> 0   -> 99990 -> 09999 -> 9999
    */
    // one or more last digits are 0
    atom_t* const reversed = array_reverse(n, len);
    atom_t* const z = zalloc(atom_t, 1);
    const uint16_t count_leading_zeroes = array_span(reversed, len, true, z, 1);
    free(z);
    const uint16_t count_nonzeroes = (uint16_t) (len - count_leading_zeroes);
    // 12300 -> 00321 -> 321
    // skip the leading zeroes when copying, allocate and copy only the "nonzero" number of digits
    atom_t* const drop_leading_zeroes = memcpy(alloc(atom_t, count_nonzeroes), reversed + count_leading_zeroes, count_nonzeroes);
    free(reversed);
    // take 1 from the first reversed nonzero digit
    drop_leading_zeroes[0] = (atom_t) (drop_leading_zeroes[0] - 1);
    // space for 00321
    atom_t* const reversed_emplace_nines = alloc(atom_t, len);
    // 00321 -> 99221
    memcpy(reversed_emplace_nines + count_leading_zeroes, drop_leading_zeroes, count_nonzeroes);
    free(drop_leading_zeroes);
    memset(reversed_emplace_nines, 9, count_leading_zeroes);
    atom_t* const pred_with_leading_zero = array_reverse(reversed_emplace_nines, len);
    free(reversed_emplace_nines);
    atom_t* final = array_trim_leading_zeroes(pred_with_leading_zero, len);
    free(pred_with_leading_zero);
    const bool shortened = (1 == n[0]) && (1 == count_nonzeroes);
    set_out_param(out_len, (uint16_t) (len - shortened));
    return final;
  }
}

static atom_t* impl_pred_b10_flot (const atom_t* const n, const uint16_t len, const uint16_t int_len, const uint16_t precision, uint16_t* const out_len) {
  (void) n; (void) len; (void) int_len; (void) precision; (void) out_len;
  return NULL;
}
/*
  atom_t*, uint16_t -> atom_t*, uint16_t

  the number before n
    before 0 is 0
    before 1 is 0
    before 9 is 8
    before 10 is 9
    before 11 is 10
    before 20 is 19
    before 100 is 99

    before 1.23 pr=2 is 1.22
    before 1.23 pr=1 is 1.13
    before 1.20 pr=0 is 0.23
*/
atom_t* pred_b10 (const atom_t* const n, const uint16_t len, const uint16_t int_len, const uint16_t precision, uint16_t* const out_len) {
  if ( raw_is_zero(n, len) ) {
    set_out_param(out_len, 0);
    return zalloc(atom_t, 1);
  }

  if (len == int_len || 0 == precision) {
    return impl_pred_b10_int(n, len, out_len);
  } else {
    return impl_pred_b10_flot(n, len, int_len, precision, out_len);
  }
}

/*
  atom_t*, uint16_t -> atom_t*, uint16_t
*/
/*atom_t* succ_b10 (const atom_t* const n, const uint16_t len, const uint16_t int_len, const uint16_t precision, uint16_t* const out_len) {
  if ( raw_is_zero(a, len) ) {
    set_out_param(out_len, 0);
    return zalloc(atom_t, 1);
  }


}
*/
/*static const atom_t euler_constant[] = { 2, 7, 1, 8, 2, 8, 1, 8, 2, 8, 4, 5, 9, 0, 4, 5, 2, 3, 5, 3, 6, 0, 2, 8, 7, 4, 7, 1, 3, 5, 2, 6, 6, 2, 4, 9, 7, 7, 5, 7, 2, 4, 7, 0, 9, 3, 6, 9, 9, 9, 5, 9, 5, 7, 4, 9, 6, 6
};
static const uint16_t euler_constant_int_len = 1;
static const uint16_t euler_constant_len = 58;
*/
/*// i would like 10 digits of precision. is that too much to ask?
atom_t* log_b10 (const atom_t* const n, const uint16_t len, uint16_t* const out_len) {
  // log 0 == inf == 0
  if ( raw_is_zero(n, len) ) {
    errno = EINVAL;
    set_out_param(out_len, 0);
    return zalloc(atom_t, 1);
  }
  uint16_t pred_len = 0;
  atom_t* const pred_n = pred_b10(n, len, len, 0, &pred_len);
  // log 1 == 0
  if ( raw_is_zero(n, pred_len) ) {
    free(pred_n);
    set_out_param(out_len, 0);
    return zalloc(atom_t, 1);
  }

  // END GUARDS
  return NULL;
}
*/
#endif /* end of include guard: MATH_PRIMITIVE_BASE10 */
