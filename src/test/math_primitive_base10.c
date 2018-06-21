#include <criterion/criterion.h>

#include "../lib/bn_common.h"

Test(mathpr_b10, int_pred) {
  const atom_t a[] = { 1 };
  atom_t* f = pred_b10(a, 1, 1, 0, NULL, NULL);
  cr_assert_eq(0, f[0]);
  free(f);

  const atom_t b[] = { 2 };
  f = pred_b10(b, 1, 1, 0, NULL, NULL);
  cr_assert_eq(1, f[0]);
  free(f);

  const atom_t c[] = { 3 };
  f = pred_b10(c, 1, 1, 0, NULL, NULL);
  cr_assert_eq(2, f[0]);
  free(f);

  const atom_t d[] = { 1, 2, 3, 0, 0 };
  f = pred_b10(d, 5, 5, 0, NULL, NULL);
  const atom_t d1[] = { 1, 2, 2, 9, 9 };
  cr_assert_arr_eq(d1, f, 5);
  free(f);

  const atom_t e[] = { 1, 0, 0, 0 };
  f = pred_b10(e, 4, 4, 0, NULL, NULL);
  const atom_t e1[] = { 9, 9, 9 };
  cr_assert_arr_eq(e1, f, 3);
  free(f);
}

Test(mathpr_b10, flot_pred) {
  const atom_t a[] = { 1, 2, 3 };
  atom_t* f = pred_b10(a, 3, 1, 2, NULL, NULL);
  const atom_t a1[] = { 1, 2, 2 };
  cr_assert_arr_eq(a1, f, 3);
  free(f);
}

Test(mathpr_b10, int_succ) {
  const atom_t a[] = { 1, 2, 3 };
  atom_t* f = succ_b10(a, 3, 3, 0, NULL, NULL);
  const atom_t a1[] = { 1, 2, 4 };
  cr_assert_arr_eq(a1, f, 3);
  free(f);
}
