#include <criterion/criterion.h>

#include "../lib/bn_common.h"

Test(b256_util, count) {
  cr_assert_eq(1, count_b256_digits_u64(1) );
  cr_assert_eq(1, count_b256_digits_u64(20) );
  cr_assert_eq(2, count_b256_digits_u64(12345) );
  cr_assert_eq(3, count_b256_digits_u64(200000) );
  cr_assert_eq(8, count_b256_digits_u64(12345678901234567890U) );
}

/* base conversions from base 10 t 256 and back */
Test(b256_to_10, u64) {
  static const atom_t d[] = {255, 255};

  const uint64_t f = b256_to_u64_digits(d, 2);

  //printf("%" PRIu64 "\n", f);

  cr_assert((255* 256) + 255 == f);
}

Test(b256_to_10, ldbl) {
  static const atom_t d[] = {255, 255};

  uint16_t il;

  const char* const f = b256_to_ldbl_digits(d, 2, 2, &il);

  //printf("%s\n", f);

  cr_assert_str_eq(f, "65535.");
}

