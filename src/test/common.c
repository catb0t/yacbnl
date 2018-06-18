#include <criterion/criterion.h>

#include "../lib/bn_common.h"

Test(common, countint) {
  cr_assert_eq(1,  count_digits_u64(1) );
  cr_assert_eq(2,  count_digits_u64(20) );
  cr_assert_eq(6,  count_digits_u64(200000) );
  cr_assert_eq(20, count_digits_u64(12345678901234567890U) );
}

Test(common, idx) {
  cr_assert_eq(0,  indexable_digits_u64(1) );
  cr_assert_eq(1,  indexable_digits_u64(20) );
  cr_assert_eq(4,  indexable_digits_u64(12345) );
  cr_assert_eq(5,  indexable_digits_u64(200000) );
  cr_assert_eq(19, indexable_digits_u64(12345678901234567890U) );
}

Test(common, countfrac) {
  cr_assert_eq(0, count_frac_digits("1234545"));
  cr_assert_eq(1, count_frac_digits("1234545."));
  cr_assert_eq(1, count_frac_digits("1234545.4"));
  cr_assert_eq(2, count_frac_digits("12345123.22"));
  cr_assert_eq(7, count_frac_digits("153452.2352344"));
}

Test(common, getn) {
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

Test(common, trimz) {
  static const atom_t a[] = {0, 1};
  atom_t* a1 = array_trim_leading_zeroes_simple(a, 2, NULL);
  cr_assert(a1[0] == 1);
  free(a1);

  static const atom_t b[] = {1};
  a1 = array_trim_leading_zeroes_simple(b, 1, NULL);
  cr_assert(a1[0] == 1);
  free(a1);

  static const atom_t c[] = {0, 0, 1};
  a1 = array_trim_leading_zeroes_simple(c, 3, NULL);
  cr_assert(a1[0] == 1);
  free(a1);
}

Test(common, concat) {
  atom_t* a = alloc(atom_t, 0);
  atom_t* a1 = alloc(atom_t, 0);
  atom_t* t = array_concat(a, 0, a1, 0);
  cr_assert(NULL != t);

  free(t);
  static const atom_t b[1] = {0};
  static const atom_t b1[1] = {0};
  t = array_concat(b, 1, b1, 1);
  say_atom_t_ptr(t, 2);
  cr_assert(t[0] == 0 && t[1] == 0);

  free(t);
  atom_t* d = alloc(atom_t, 0);
  static const atom_t d1[1] = {0};
  t = array_concat(d, 0, d1, 1);
  say_atom_t_ptr(t, 1);
  cr_assert(t[0] == 0);

  free(t);
  t = array_concat(d1, 1, d, 0);
  say_atom_t_ptr(t, 1);
  cr_assert(t[0] == 0);

  free(t);
  static const atom_t c[4] = {100, 200, 123, 0};
  static const atom_t c1[3] = {5, 6, 7};
  t = array_concat(c, 4, c1, 3);
  say_atom_t_ptr(t, 7);
  static const atom_t cres[] = {100, 200, 123, 0, 5, 6, 7};
  cr_assert_arr_eq(cres, t, 7);
}
