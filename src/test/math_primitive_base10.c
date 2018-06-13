#include <criterion/criterion.h>

#include "../lib/bn_common.h"

Test(mathpr_b10, int_pred) {
  const atom_t a[] = { 1 };
  atom_t* f = pred_b10(a, 1, 1, 0, NULL);
  cr_assert_eq(0, f[0]);
  free(f);
}
