#include <criterion/criterion.h>

#include "../lib/bn_common.h"

/* base 256 single byte addressing */
Test(a1b256, bna_u64) {

  // zero
  atom_t
    * a = to_digit_array(0, 0, FL_NONE, TYP_ZENZ),
    z[HEADER_OFFSET] = { TYP_ZENZ, 0, FL_NONE, 0 };

  cr_assert_arr_eq(z, a, sz(atom_t, HEADER_OFFSET));
  free(a);

  // 1
  a = to_digit_array(0, 1, FL_NONE, TYP_ZENZ);
  atom_t s[HEADER_OFFSET + 1] = { TYP_ZENZ, 1, 0, FL_NONE, 1 };
  say_atom_t_ptr(a, HEADER_OFFSET + 1);

  cr_assert_arr_eq(s, a, sz(atom_t, 1 + HEADER_OFFSET));
  free(a);

  // 23
  a = to_digit_array(0, 23, FL_NONE, TYP_ZENZ);
  atom_t s2[HEADER_OFFSET + 1] = { TYP_ZENZ, 1, 0, FL_NONE, 23 };

  cr_assert_arr_eq(s2, a, sz(atom_t, 1 + HEADER_OFFSET));
  free(a);

  // long
  a = to_digit_array(0, 12345678901234567890U, FL_NONE, TYP_ZENZ);
  atom_t s3[HEADER_OFFSET + 8] = { TYP_ZENZ, 8, 0, FL_NONE, 171, 84, 169, 140, 235, 31, 10, 210 };

  cr_assert_arr_eq(s3, a, sz(atom_t, 8 + HEADER_OFFSET));
  free(a);

}

Test(a1b256, bna_s64) {
  atom_t
    * a = to_digit_array(0, 0, FL_SIGN, TYP_ZENZ),
    z[HEADER_OFFSET] = { TYP_ZENZ, 0, 0, FL_SIGN };

  cr_assert_arr_eq(z, a, sz(atom_t, HEADER_OFFSET));
  free(a);

  a = to_digit_array(0, 1, FL_SIGN, TYP_ZENZ);
  atom_t n1[HEADER_OFFSET + 1] = { TYP_ZENZ, 1, 0, FL_SIGN, 1 };

  cr_assert_arr_eq(n1, a, sz(atom_t, 1 + HEADER_OFFSET));
  free(a);

  a = to_digit_array(0, 2345, FL_SIGN, TYP_ZENZ);
  atom_t n2[HEADER_OFFSET + 2] = { TYP_ZENZ, 2, 0, FL_SIGN, 9, 41 };

  cr_assert_arr_eq(n2, a, sz(atom_t, 2 + HEADER_OFFSET));
  free(a);

  a = to_digit_array(0, 12345678901234567890U, FL_SIGN, TYP_ZENZ);
  atom_t s3[HEADER_OFFSET + 8] = { TYP_ZENZ, 8, 0, FL_SIGN, 171, 84, 169, 140, 235, 31, 10, 210 };

  cr_assert_arr_eq(s3, a, sz(atom_t, 8 + HEADER_OFFSET));
  free(a);

}
