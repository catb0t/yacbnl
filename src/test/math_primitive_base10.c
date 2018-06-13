#include <criterion/criterion.h>

#include "../lib/bn_common.h"

Test(mathpr_b10, int_pred) {
  const atom_t a[] = { 1 };
  atom_t* f = pred_b10(a, 1, 1, 0, NULL);
  cr_assert_eq(0, f[0]);
  free(f);

  const atom_t b[] = { 2 };
  f = pred_b10(b, 1, 1, 0, NULL);
  cr_assert_eq(1, f[0]);
  free(f);

  const atom_t c[] = { 3 };
  f = pred_b10(c, 1, 1, 0, NULL);
  cr_assert_eq(2, f[0]);
  free(f);

  const atom_t d[] = { 1, 2, 3, 0, 0 };
  f = pred_b10(d, 5, 5, 0, NULL);
  const atom_t d1[] = { 1, 2, 2, 9, 9 };
  say_atom_t_ptr(d, 5);
  say_atom_t_ptr(d1, 5);
  say_atom_t_ptr(f, 5);
  cr_assert_arr_eq(d1, f, 5);
  free(f);
}
