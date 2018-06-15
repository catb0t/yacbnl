#ifndef MATH_PRIMITIVE_BASE10
#define MATH_PRIMITIVE_BASE10

#include "bn_common.h"
/* simple unsigned real number math */


static atom_t* impl_pred_b10_int (const atom_t* const n, const uint16_t len, uint16_t* const out_len) {
  // last digit is not 0
  if (0 != n[len - 1]) {
    set_out_param(out_len, len);
    // only the last digit changes, so copy all the preceding digits
    atom_t* const result = (atom_t*) memcpy(alloc(atom_t, len), n, (uint16_t) (len - 1));
    // then take 1
    result[len - 1] = (atom_t) ((atom_t) n[len - 1] - 1);
    return result;
  } else {

    // TODO: USE array_concat
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
    atom_t* const leading_digits = (atom_t*) memcpy(alloc(atom_t, count_leading_nonzeroes), n, count_leading_nonzeroes);
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
    atom_t* const result = (atom_t*) memcpy(alloc(atom_t, len), n, len);
    // then take 1
    result[focus_digit] = (atom_t) (n[focus_digit] - 1);
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

static atom_t* impl_succ_b10_int (const atom_t* const n, const uint16_t len, uint16_t* const out_len) {
  puts("UNIMPLEMENTED");
  (void) n;
  (void) len;
  (void) out_len;
  return NULL;
}

/*
  atom_t*, uint16_t -> atom_t*, uint16_t
*/
atom_t* succ_b10 (const atom_t* const n, const uint16_t len, const uint16_t int_len, const uint16_t precision, uint16_t* const out_len, uint16_t* const out_int_len) {
  if ( raw_is_zero(n, len) ) {
    set_out_param(out_len, 1);
    atom_t* const res = alloc(atom_t, 1);
    res[0] = 1;
    return res;
  }
  (void) precision;
  (void) int_len;
  (void) out_int_len;
  return NULL;
}

atom_t* add_b10 (const atom_t* const a, const uint16_t a_len, const uint16_t a_int_len, const atom_t* const b, const uint16_t b_len, const uint16_t b_int_len, uint16_t* const out_len, uint16_t* const out_int_len) {
  puts("UNIMPLEMENTED");
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

atom_t* sub_b10 (const atom_t* const a, const uint16_t a_len, const uint16_t a_int_len, const atom_t* const b, const uint16_t b_len, const uint16_t b_int_len, uint16_t* const out_len, uint16_t* const out_int_len) {
  puts("UNIMPLEMENTED");
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

atom_t* mul_b10 (const atom_t* const a, const uint16_t a_len, const uint16_t a_int_len, const atom_t* const b, const uint16_t b_len, const uint16_t b_int_len, uint16_t* const out_len, uint16_t* const out_int_len) {
  puts("UNIMPLEMENTED");
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

atom_t* div_b10 (const atom_t* const a, const uint16_t a_len, const uint16_t a_int_len, const atom_t* const b, const uint16_t b_len, const uint16_t b_int_len, uint16_t* const out_len, uint16_t* const out_int_len) {
  puts("UNIMPLEMENTED");
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

atom_t* pow_b10 (const atom_t* const a, const uint16_t a_len, const uint16_t a_int_len, const atom_t* const b, const uint16_t b_len, const uint16_t b_int_len, uint16_t* const out_len, uint16_t* const out_int_len) {
  puts("UNIMPLEMENTED");
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

atom_t* times2_b10 (const atom_t* const n, const uint16_t len, const uint16_t int_len, uint16_t* const out_len, uint16_t* const out_int_len) {
  puts("UNIMPLEMENTED");
  (void) n;
  (void) len;
  (void) int_len;
  (void) out_len;
  (void) out_int_len;
  return NULL;
}

atom_t* sq_b10 (const atom_t* const n, const uint16_t len, const uint16_t int_len, uint16_t* const out_len, uint16_t* const out_int_len) {
  puts("UNIMPLEMENTED");
  (void) n;
  (void) len;
  (void) int_len;
  (void) out_len;
  (void) out_int_len;
  return NULL;
}
atom_t* recip_b10 (const atom_t* const n, const uint16_t len, const uint16_t int_len, uint16_t* const out_len, uint16_t* const out_int_len) {
  puts("UNIMPLEMENTED");
  (void) n;
  (void) len;
  (void) int_len;
  (void) out_len;
  (void) out_int_len;
  return NULL;
}

atom_t* floor_b10 (const atom_t* const n, const uint16_t len, const uint16_t int_len, uint16_t* const out_len, uint16_t* const out_int_len) {
  puts("UNIMPLEMENTED");
  (void) n;
  (void) len;
  (void) int_len;
  (void) out_len;
  (void) out_int_len;
  return NULL;
}

// i would like 10 digits of precision. is that too much to ask?
atom_t* impl_log_b10(const atom_t* const n, const uint16_t n_len, const uint16_t n_int_len, uint16_t* const out_len, uint16_t* const out_int_len, const uint16_t iterations) {

  /*
    https://stackoverflow.com/questions/46879166
    taylor

    uses: pred succ addition subtraction division multiplication reciprocal times2
  */

  /* out parameters for array lengths */

  uint16_t
    total_len = 0, total_int_len = 0,
    n_succ_len = 0, n_succ_int_len = 0,
    n_succ_sq_len = 0, n_succ_sq_int_len = 0,
    n_pred_len = 0, n_pred_int_len = 0,
    n_pred_sq_len = 0, n_pred_sq_int_len = 0,
    run_mul_len = 0, run_mul_int_len = 0,
    power_len = 0, power_int_len = 0,
    y_len = 0, y_int_len = 0;

  /*
    number variable declarations

    total = 0, z = (x + 1) / (x - 1), power = 1, y = 0;
  */
  atom_t* const zero = zalloc(atom_t, 1),
        * total = array_copy(zero, 1),
        * power = succ_b10(zero, 1, 1, 0, &power_len, &power_int_len),
        * y = array_copy(zero, 1),

        * const n_succ = succ_b10(n, n_len, n_int_len, 0, &n_succ_len, &n_succ_int_len),
        * const n_succ_sq = sq_b10(n_succ, n_succ_len, n_succ_int_len, &n_succ_sq_len, &n_succ_sq_int_len),
        * const n_pred = pred_b10(n, n_len, n_int_len, 0, &n_pred_len, &n_pred_int_len),
        * const n_pred_sq = sq_b10(n_pred, n_pred_len, n_pred_int_len, &n_pred_sq_len, &n_pred_sq_int_len),

        * run_mul = div_b10(n_succ, n_succ_len, n_succ_int_len, n_pred, n_pred_len, n_pred_int_len, &run_mul_len, &run_mul_int_len);

  for (uint16_t i = 0; i < iterations; i++) {
    free(y);
    /* STEP 1: z *= ((x - 1) * (x - 1)) / ((x + 1) * (x + 1)); */

    // going to do run_mul = run_mul * N
    const uint16_t temp_run_mul_len = run_mul_len, temp_run_mul_int_len = run_mul_int_len;
    atom_t* const temp_run_mul = array_copy(run_mul, run_mul_len);
    free(run_mul); // going to re-fill this with the new value
    // run_mul = run_mul * ((x - 1) * (x - 1)) / ((x + 1) * (x + 1));
    // AKA run_mul = run_mul * ((x - 1)^2) / ((x + 1)^2);
    uint16_t mul_step_len = 0, mul_step_int_len = 0;
    atom_t* const mul_step = div_b10(n_pred_sq, n_pred_sq_len, n_pred_sq_int_len, n_succ_sq, n_succ_sq_len, n_succ_sq_len, &mul_step_len, &mul_step_int_len);
    run_mul = mul_b10(temp_run_mul, temp_run_mul_len, temp_run_mul_int_len, mul_step, mul_step_len, mul_step_int_len, &run_mul_len, &run_mul_int_len);
    free(mul_step);
    free(temp_run_mul);

    /* STEP 2: y = recip(power) * z */

    uint16_t recip_power_len = 0, recip_power_int_len = 0;
    atom_t* const recip_power = recip_b10(power, power_len, power_int_len, &recip_power_len, &recip_power_int_len);
    y = mul_b10(recip_power, recip_power_len, recip_power_int_len, run_mul, run_mul_len, run_mul_int_len, &y_len, &y_int_len);
    free(recip_power);

    /* STEP 3: total += y */
    total = add_b10(total, total_len, total_int_len, y, y_len, y_int_len, &total_len, &total_int_len);

    /* STEP 4: power += 2 */

    // abuse impl_succ_b10_int because we are always adding integer 2
    power = impl_succ_b10_int(power, power_len, &power_len);
    power_int_len = power_len;
    power = impl_succ_b10_int(power, power_len, &power_len);
    power_int_len = power_len;
  }

  free(zero);
  free(total);
  free(power);
  free(y);
  free(n_succ);
  free(n_succ_sq);
  free(n_pred);
  free(n_pred_sq);
  free(run_mul);

  atom_t* const final = times2_b10(total, total_len, total_int_len, out_len, out_int_len);
  free(total);
  return final;
}

atom_t* logn_b10 (const atom_t* const base, const uint16_t base_len, const uint16_t base_int_len, const atom_t* const n, const uint16_t n_len, const uint16_t n_int_len, uint16_t* const out_len, uint16_t* const out_int_len) {
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
