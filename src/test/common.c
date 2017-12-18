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

