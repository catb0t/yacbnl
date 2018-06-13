#include <criterion/criterion.h>

#include "../lib/bn_common.h"

Test(b256_util, count) {
  cr_assert_eq(1, count_b256_digits_u64(1) );
  cr_assert_eq(1, count_b256_digits_u64(20) );
  cr_assert_eq(2, count_b256_digits_u64(12345) );
  cr_assert_eq(3, count_b256_digits_u64(200000) );
  cr_assert_eq(8, count_b256_digits_u64(12345678901234567890U) );

  cr_assert_eq(1, count_b256_digits_b10_digits("1") );
  cr_assert_eq(1, count_b256_digits_b10_digits("20") );
  cr_assert_eq(2, count_b256_digits_b10_digits("12345") );
  cr_assert_eq(3, count_b256_digits_b10_digits("200000") );
  cr_assert_eq(8, count_b256_digits_b10_digits("12345678901234567890") );
}

/* base conversions from base 10 t 256 and back */
Test(b256_to_b10, u64) {
  static const atom_t d[] = {255, 255};

  const uint64_t f = b256_to_u64(d, 2);

  //printf("%" PRIu64 "\n", f);

  cr_assert((255 * 256) + 255 == f);
}

Test(b256_to_b10, ldbl) {
  static const atom_t d[] = {255, 255};

  char* const f = b256_to_ldbl_digits(d, 1, 1);

  //printf("%s\n", f);

  cr_assert_str_eq(f, "65535.");
  free(f);
}

Test(b10_to_b256, u64) {
  const atom_t a[1] = { 1 };
  uint16_t len = 0;

  atom_t* const d = u64_digits_to_b256("1", &len, false);
  cr_assert_eq(len, 1);
  cr_assert_arr_eq(a, d, len);
  free(d);

  len = 0;
  atom_t* const f = u64_to_b256(a[0], &len, false);
  cr_assert_eq(len, 1);
  cr_assert_arr_eq(a, f, len);
  free(f);
}
