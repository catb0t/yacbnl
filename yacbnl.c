#include "yacbnl.h"

int main(void) {

  uint16_t* a = to_bn_array(0.f, 1);

  printf("%hu %hu %hu\n", a[0], a[1], a[2]);
  for (uint16_t i = 2; i < a[0] + 1; i++) {
    //printf("%hu ", a[i]);
  }
  puts("");

  free(a);

  return EXIT_SUCCESS;

  uint16_t* b = to_bn_array(1234.567, 0);

  for (size_t i = 0; i < b[0]; i++) {
    printf("%hu ", a[i]);
  }
  return 0;
}
