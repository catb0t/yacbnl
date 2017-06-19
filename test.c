#include "base256.h"

int main(void) {
/*  atom_t a[] = { 184, 255 };
  printf("%" PRIu64 "\n", b256_to_u64_digits(a, 3));
*/
  size_t len;
  atom_t* r = u64_digits_to_b256(65464, &len);
  for (size_t i = 0; i < len; i++) {
    printf("%d\n", r[i]);
  }
  return 0;
}
