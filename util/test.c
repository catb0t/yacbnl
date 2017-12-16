#include "../addr_interp.h"
#include "../yacbnl.h"

int main(void) {

  printf("%d\n", 001);
  atom_t
    * a = to_digit_array(0, 1, FL_NONE, TYP_ZENZ),
    z[HEADER_OFFSET + 4] = { TYP_ZENZ, 1, 0, FL_NONE, 1 };

  printf("a: %d %d %d\n", a[0], a[1], a[2]);
  printf("z: %d %d %d\n", z[0], z[1], z[2]);

  free(a);
/*  atom_t a[] = { 184, 255 };
  printf("%" PRIu64 "\n", b256_to_u64_digits(a, 3));
  size_t len;
  atom_t* r = u64_digits_to_b256(65464, &len);
  for (size_t i = 0; i < len; i++) {
    printf("%d\n", r[i]);
  }
*/    return 0;
}
