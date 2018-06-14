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
      12300 -> 123 -> 122 -> 12299
      12100 -> 121 -> 120 -> 12099
      50000 -> 5 -> 4 -> 49999
      10000 -> 1 -> 0 -> 09999 -> 9999
    */
    // one or more last digits are 0
    atom_t* const z = zalloc(atom_t, 1);
    const uint16_t count_leading_nonzeroes = array_span(n, len, false, z, 1);
    free(z);
    const uint16_t count_trailing_zeroes = (uint16_t) (len - count_leading_nonzeroes);
    atom_t* const leading_digits = memcpy(alloc(atom_t, count_leading_nonzeroes), n, count_leading_nonzeroes);
    // take 1 from the first nonzero digit
    leading_digits[count_leading_nonzeroes - 1] = (atom_t) (leading_digits[count_leading_nonzeroes - 1] - 1);
    // space for 00321
    atom_t* const emplace_nines = alloc(atom_t, len);
    // 00321 -> 99221
    memcpy(emplace_nines, leading_digits, count_leading_nonzeroes);
    free(leading_digits);
    // emplace the needed number of nines
    memset(emplace_nines + count_leading_nonzeroes, 9, count_trailing_zeroes);
    // reverse: 99221 -> 12299
    // drop any leftover leading zeroes: 0999 -> 999
    atom_t* const final = array_trim_leading_zeroes_simple(emplace_nines, len);
    free(emplace_nines);
    // record if it was shortened by a digit
    const bool shortened = (1 == n[0]) && (1 == count_leading_nonzeroes);
    set_out_param(out_len, (uint16_t) (len - shortened));
    return final;
  }
}

static atom_t* impl_pred_b10_flot (const atom_t* const n, const uint16_t len, const uint16_t int_len, const uint16_t precision, uint16_t* const out_len, uint16_t* const out_int_len) {
  // going to traverse one more than int_len
  // preicision may be 0 to subtract 1 from a real
  const uint16_t focus_digit = (uint16_t) ((int_len + precision) - 1);
  if ( 0 != n[focus_digit] ) {
    // simple case: digit of interest is not 0 so just take 1
    set_out_param(out_len, len);
    set_out_param(out_int_len, int_len);
    atom_t* const result = memcpy(alloc(atom_t, len), n, len);
    // then take 1
    result[focus_digit] = (atom_t) ((atom_t) n[focus_digit] - 1);
    return result;
  } else {
    // the digit in focus is 0
    // we need to figure out what other digits are 0
    // taking 1 from a digit only affects the digits more significant than it
    puts("UNIMPLEMENTED");

    return NULL;
  }
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
    before 1.01 pr=1 is 0.91
    before 0.99 pr=2 is 0.98
*/
atom_t* pred_b10 (const atom_t* const n, const uint16_t len, const uint16_t int_len, const uint16_t precision, uint16_t* const out_len, uint16_t* const out_int_len) {
  if ( (len == int_len && raw_is_zero(n, int_len) ) || raw_is_zero(n, len) ) {
    set_out_param(out_len, 0);
    return zalloc(atom_t, 1);
  }

  if (len == int_len && 0 == precision) {
    return impl_pred_b10_int(n, len, out_len);
  } else {
    return impl_pred_b10_flot(n, len, int_len, precision, out_len, out_int_len);
  }
}

/*
  atom_t*, uint16_t -> atom_t*, uint16_t
*/
atom_t* succ_b10 (const atom_t* const n, const uint16_t len, const uint16_t int_len, const uint16_t precision, uint16_t* const out_len, uint16_t* const out_int_len) {
  if ( raw_is_zero(n, len) ) {
    set_out_param(out_len, 0);
    return zalloc(atom_t, 1);
  }
  (void) precision;
  (void) int_len;
  (void) out_int_len;
  return NULL;
}

atom_t* div_b10 (const atom_t* const a, const uint16_t a_len, const uint16_t a_int_len, const atom_t* const b, const uint16_t b_len, const uint16_t b_int_len, uint16_t* const out_len, uint16_t* const out_int_len) {
  (void) a;
  (void) a_len;
  (void) a_int_len;
  (void) b;
  (void) b_len;
  (void) b_int_len;
  (void) out_len;
  (void) out_int_len;
  return NULL;
}

atom_t* floor_b10 (const atom_t* const n, const uint16_t len, const uint16_t int_len) {
  (void) n;
  (void) len;
  (void) int_len;
  return NULL;
}

/*static const atom_t euler_constant[] = { 2, 7, 1, 8, 2, 8, 1, 8, 2, 8, 4, 5, 9, 0, 4, 5, 2, 3, 5, 3, 6, 0, 2, 8, 7, 4, 7, 1, 3, 5, 2, 6, 6, 2, 4, 9, 7, 7, 5, 7, 2, 4, 7, 0, 9, 3, 6, 9, 9, 9, 5, 9, 5, 7, 4, 9, 6, 6
};
static const uint16_t euler_constant_int_len = 1;
static const uint16_t euler_constant_len = 58;
*/
// i would like 10 digits of precision. is that too much to ask?
atom_t* log_b10(const atom_t* const n, const uint16_t n_len, const uint16_t n_int_len, uint16_t* const out_len, uint16_t* const out_int_len) {
  /* who knows??? */
  puts("UNIMPLEMENTED");
  (void) n;
  (void) n_int_len;
  (void) n_len;
  (void) out_int_len;
  (void) out_len;
  return NULL;
}

atom_t* logn_b10 (const atom_t* const base /* n */, const uint16_t base_len, const uint16_t base_int_len, const atom_t* const n /* x */, const uint16_t n_len, const uint16_t n_int_len, /* out */ uint16_t* const out_len, /* out */ uint16_t* const out_int_len) {
  uint16_t log_base_int_len = 0, log_base_len = 0;
  atom_t* const log_base = log_b10(base, base_len, base_int_len, &log_base_len, &log_base_int_len);

  uint16_t log_n_int_len = 0, log_n_len = 0;
  atom_t* const log_n = log_b10(n, n_len, n_int_len, &log_n_len, &log_n_int_len);

  atom_t* const result = div_b10(log_n, log_n_len, log_n_int_len, log_base, log_base_len, log_base_int_len, out_len, out_int_len);
  free(log_base);
  free(log_n);
  return result;
}

#endif /* end of include guard: MATH_PRIMITIVE_BASE10 */
