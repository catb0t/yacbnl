#include "yacbnl.h"

int main(void) {

  uint8_t* a = to_bn_array(0, 0, FL_SIGN);
  free(a);

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
*/}
