#include <criterion/criterion.h>

#include "../yacbnl.h"

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

