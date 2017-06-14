#include <criterion/criterion.h>

#include "yacbnl.h"

Test(t, count) {
  cr_assert_eq(1, count_digits_u64(1) );
  cr_assert_eq(2, count_digits_u64(20) );
  cr_assert_eq(6, count_digits_u64(200000) );
}

Test(t, getn) {
  cr_assert_eq(1, get_left_nth_digit(12345, 0));
  cr_assert_eq(2, get_left_nth_digit(12345, 1));
  cr_assert_eq(3, get_left_nth_digit(12345, 2));
  cr_assert_eq(4, get_left_nth_digit(12345, 3));
  cr_assert_eq(5, get_left_nth_digit(12345, 4));
}
