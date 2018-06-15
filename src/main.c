#include "yacbnl.c"
#include <float.h>

int main(void) {
  puts("main");

  //const ldbl_t max = LDBL_MAX;

  //printf("%Lf\n", max);
  //printf("%LG\n", max);

  uint16_t len, int_len;

  atom_t* const ds = ldbl_digits_to_b256("340282366920938463463374607431768211455.0", &len, &int_len, false);

  for (size_t i = 0; i < len; i++) {
    printf("%d", ds[i]);
  }

  puts("");
/*  atom_t* a = bna_from_flot(1.234f, BN_NONE);
  free(a);
*/  //printf("%d\n", count_frac_digits("1234.0002"));
/*  uint8_t* a = to_bn_array(0, 12345678901234567890U, FL_SIGN);
  for (atom_t i = 0; i < a[0] + 3; i++) {
    printf("%d ", a[i]);
  }
  free(a);
*/
/*  printf("%d %d\n", count_digits_u64(12345678901234567890U), get_left_nth_digit(12345678901234567890U, 19));

  uint8_t* a = to_bn_array(0.f, 12345678901234567890U, 0);

  printf("%d %d\n", a[0], a[1]);
  for (uint8_t i = 2; i < a[0] + 2; i++) {
    printf("%d ", a[i]);
  }
  puts("");
  free(a);

  return EXIT_SUCCESS;
*/
/*  uint8_t* b = to_bn_array(1234.567, 0, 0);

  for (size_t i = 0; i < b[0]; i++) {
    printf("%d ", a[i]);
  }
  return EXIT_SUCCESS;
*/
}
