#include "../addr_interp.h"
#include "../yacbnl.h"

int main(void) {
  atom_t
    * a = to_digit_array((ldbl_t) 1.234, 0, FL_NONE, 0),
    z[HEADER_OFFSET + 4] = { 0, 1, 3, 0, 1, 2, 3, 4 };

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
  */
  return 0;
}
