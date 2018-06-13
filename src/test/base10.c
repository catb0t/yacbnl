#include <criterion/criterion.h>

#include "../lib/bn_common.h"

/* base 10 conversions to/from */

Test(base10, strton) {
  const atom_t a[] = { 1, 2, 3, 4, 5 };

  char* ar = b10_to_ldbl_digits(NULL, 3, 3);
  cr_assert_str_eq(ar, "");
  free(ar);

  ar = b10_to_ldbl_digits(a, 0, 1);
  cr_assert_str_eq(ar, "");
  free(ar);

  ar = b10_to_ldbl_digits(a, 3, 3);
  cr_assert_str_eq(ar, "123");
  free(ar);

  ar = b10_to_ldbl_digits(a, 5, 3);
  cr_assert_str_eq(ar, "123.45");
  free(ar);

  //b10_to_u64(const atom_t *const digits, const uint16_t len);
  //b10_to_u64_digits(const atom_t *const digits, const uint16_t len)
}
