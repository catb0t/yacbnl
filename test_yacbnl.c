#include <criterion/criterion.h>

#include "yacbnl.h"

Test(t, count) {
  cr_assert_eq(1,  count_digits_u64(1) );
  cr_assert_eq(2,  count_digits_u64(20) );
  cr_assert_eq(6,  count_digits_u64(200000) );
  cr_assert_eq(20, count_digits_u64(12345678901234567890U) );
}

Test(t, idx) {
  cr_assert_eq(0,  indexable_digits_u64(1) );
  cr_assert_eq(1,  indexable_digits_u64(20) );
  cr_assert_eq(4,  indexable_digits_u64(12345) );
  cr_assert_eq(5,  indexable_digits_u64(200000) );
  cr_assert_eq(19, indexable_digits_u64(12345678901234567890U) );
}

Test(t, getn) {
  uint64_t x = 12345;
  for (size_t i = 0; i < 5; i++) {
    atom_t res = get_left_nth_digit(x, (atom_t) i);
    //printf("%zu, %zu, %d\n", i, i + 1, res);
    cr_assert_eq(i + 1, res);
  }

  x = 12345678901234567890U;
  for (size_t i = 0; i < 20; i++) {
    //printf("%ld %d\n", (i + 1) % 10, get_left_nth_digit(x, (atom_t) i));
    cr_assert_eq((i + 1) % 10, get_left_nth_digit(x, (atom_t) i));
  }
}

Test(t, bna_u64) {

  // zero
  atom_t
    * a = to_bn_array(0, 0, BN_NONE),
    z[] = { 0, 0, 0 };

  cr_assert_arr_eq(z, a, sz(HEADER_OFFSET, atom_t));
  free(a);

  // 1
  a = to_bn_array(0, 1, BN_NONE);

  atom_t s[] = { 1, 0, 0, 1 };

  cr_assert_arr_eq(s, a, sz(1 + HEADER_OFFSET, atom_t));
  free(a);

  // 23
  a = to_bn_array(0, 23, BN_NONE);
  atom_t s2[] = { 2, 0, 0, 2, 3 };

  cr_assert_arr_eq(s2, a, sz(2 + HEADER_OFFSET, atom_t));
  free(a);

  // long number
  a = to_bn_array(0, 12345678901234567890U, 0);
  atom_t s3[23] = { 20, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 };

  cr_assert_arr_eq(s3, a, sz(20 + HEADER_OFFSET, atom_t));
  free(a);
}

Test(t, bna_s64) {

  atom_t
    * a = to_bn_array(0, 0, FL_SIGN),
    z[] = { 0, 0, FL_SIGN };

  cr_assert_arr_eq(z, a, sz(HEADER_OFFSET, atom_t));
  free(a);

  a = to_bn_array(0, 1, FL_SIGN);
  atom_t n1[] = {
    1, 0, FL_SIGN, 1
  };

  cr_assert_arr_eq(n1, a, sz(1 + HEADER_OFFSET, atom_t));
  free(a);

  a = bna_from_uint(2345, FL_SIGN);
  atom_t n2[] = {
    4, 0, FL_SIGN, 2, 3, 4, 5
  };

  cr_assert_arr_eq(n2, a, sz(4 + HEADER_OFFSET, atom_t));
  free(a);
}

Test(t, bna_ldbl) {
  atom_t
    * a = to_bn_array((ldbl_t) 1.234, 0, BN_NONE),
    z[] = { 1, 3, 0, 1, 2, 3, 4 };

  printf("%d %d %d\n", a[0], a[1], a[2]);
  printf("%d %d %d\n", z[0], z[1], z[2]);

  cr_assert_arr_eq(z, a, sz(4 + HEADER_OFFSET, atom_t));

  free(a);
}
