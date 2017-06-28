#include <criterion/criterion.h>

#include "../yacbnl.h"

Test(t, bna_u64) {

  // zero
  atom_t
    * a = to_digit_array(0, 0, BN_NONE, 0),
    z[] = { 0, 0, 0, 0 };

  cr_assert_arr_eq(z, a, sz(HEADER_OFFSET, atom_t));
  free(a);

  // 1
  a = to_digit_array(0, 1, BN_NONE, 0);

  atom_t s[] = { 0, 1, 0, 0, 1 };

  cr_assert_arr_eq(s, a, sz(1 + HEADER_OFFSET, atom_t));
  free(a);

  // 23
  a = to_digit_array(0, 23, BN_NONE, 0);
  atom_t s2[] = { 2, 0, 0, 2, 3 };

  cr_assert_arr_eq(s2, a, sz(2 + HEADER_OFFSET, atom_t));
  free(a);

  // long number
  a = to_digit_array(0, 12345678901234567890U, 0, 0);
  atom_t s3[24] = { 0, 20, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 };

  cr_assert_arr_eq(s3, a, sz(20 + HEADER_OFFSET, atom_t));
  free(a);
}

Test(t, bna_s64) {

  atom_t
    * a = to_digit_array(0, 0, FL_SIGN, 0),
    z[] = { 0, 0, 0, FL_SIGN };

  cr_assert_arr_eq(z, a, sz(HEADER_OFFSET, atom_t));
  free(a);

  a = to_digit_array(0, 1, FL_SIGN, 0);
  atom_t n1[] = {
    0, 1, 0, FL_SIGN, 1
  };

  cr_assert_arr_eq(n1, a, sz(1 + HEADER_OFFSET, atom_t));
  free(a);

  a = to_digit_array(0, 2345, FL_SIGN, 0);
  atom_t n2[] = {
    0, 4, 0, FL_SIGN, 2, 3, 4, 5
  };

  cr_assert_arr_eq(n2, a, sz(4 + HEADER_OFFSET, atom_t));
  free(a);
}

Test(t, bna_ldbl) {
  atom_t
    * a = to_digit_array((ldbl_t) 1.234, 0, BN_NONE, 0),
    z[] = { 0, 1, 3, 0, 1, 2, 3, 4 };

  printf("%d %d %d\n", a[0], a[1], a[2]);
  printf("%d %d %d\n", z[0], z[1], z[2]);

  cr_assert_arr_eq(z, a, sz(4 + HEADER_OFFSET, atom_t));

  free(a);
}
