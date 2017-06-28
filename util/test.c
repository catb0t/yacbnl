#include "../yacbnl.h"

int main(void) {
  // 23
  atom_t* a = to_digit_array(0, 23, BN_NONE, 0);
  atom_t s2[] = { 2, 0, 0, 0, 2, 3 };

  free(a);


/*  atom_t a[] = { 184, 255 };
  printf("%" PRIu64 "\n", b256_to_u64_digits(a, 3));
  size_t len;
  atom_t* r = u64_digits_to_b256(65464, &len);
  for (size_t i = 0; i < len; i++) {
    printf("%d\n", r[i]);
  }
  */
  return 0;
}
