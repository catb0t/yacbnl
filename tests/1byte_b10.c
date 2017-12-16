#include <criterion/criterion.h>

#include "../yacbnl.h"

/* base 10 single byte addressing */

Test(a1b10, bna_u64) {

  // zero
  atom_t
    * a = to_digit_array(0, 0, FL_NONE, TYP_NONE),
    z[HEADER_OFFSET] = { TYP_NONE, 0, 0, 0 };

  cr_assert_arr_eq(z, a, sz(atom_t, HEADER_OFFSET));
  free(a);

  // 1
  a = to_digit_array(0, 1, FL_NONE, TYP_NONE);
  atom_t s[HEADER_OFFSET + 1] = { TYP_NONE, 1, 0, 0, 1 };

  cr_assert_arr_eq(s, a, sz(atom_t, 1 + HEADER_OFFSET));
  free(a);

  // 23
  a = to_digit_array(0, 23, FL_NONE, TYP_NONE);
  atom_t s2[HEADER_OFFSET + 2] = { TYP_NONE, 2, 0, FL_NONE, 2, 3 };

  cr_assert_arr_eq(s2, a, sz(atom_t, 2 + HEADER_OFFSET));
  free(a);

  // long number
  a = to_digit_array(0, 12345678901234567890U, FL_NONE, TYP_NONE);
  atom_t s3[HEADER_OFFSET + 20] = { TYP_NONE, 20, 0, FL_NONE, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 };

  cr_assert_arr_eq(s3, a, sz(atom_t, 20 + HEADER_OFFSET));
  free(a);
}

Test(a1b10, bna_s64) {

  atom_t
    * a = to_digit_array(0, 0, FL_SIGN, TYP_NONE),
    z[HEADER_OFFSET] = { TYP_NONE, 0, 0, FL_SIGN };

  cr_assert_arr_eq(z, a, sz(atom_t, HEADER_OFFSET));
  free(a);

  a = to_digit_array(0, 1, FL_SIGN, TYP_NONE);
  atom_t n1[HEADER_OFFSET + 1] = { TYP_NONE, 1, 0, FL_SIGN, 1 };

  cr_assert_arr_eq(n1, a, sz(atom_t, 1 + HEADER_OFFSET));
  free(a);

  a = to_digit_array(0, 2345, FL_SIGN, TYP_NONE);
  atom_t n2[HEADER_OFFSET + 4] = { TYP_NONE, 4, 0, FL_SIGN, 2, 3, 4, 5 };

  cr_assert_arr_eq(n2, a, sz(atom_t, 4 + HEADER_OFFSET));
  free(a);
}

Test(a1b10, bna_ldbl) {
  atom_t* a = to_digit_array((ldbl_t) 1.234, 0, FL_NONE, TYP_NONE),
    z[HEADER_OFFSET + 4] = { TYP_NONE, 1, 3, FL_NONE, 1, 2, 3, 4 };

  for (uint16_t i = 0; i < bna_real_len(a); i++) {
    //printf("%d ", a[i]);
  }
  cr_assert_arr_eq(z, a, sz(atom_t, 4 + HEADER_OFFSET));

  free(a);

  #define BNALDBL_LEN 3 + 3
  a = to_digit_array((ldbl_t) 155.235, 0, FL_SIGN, TYP_NONE);
  atom_t y[HEADER_OFFSET + BNALDBL_LEN] = { TYP_NONE, 3, 3, FL_SIGN, 1, 5, 5, 2, 3, 5 };

  for (uint16_t i = 0; i < bna_real_len(a); i++) {
    //printf("%d ", a[i]);
  }

  cr_assert_arr_eq(y, a, sz(atom_t, BNALDBL_LEN + HEADER_OFFSET));

  free(a);

}
