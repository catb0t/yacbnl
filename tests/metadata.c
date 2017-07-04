#include <criterion/criterion.h>

#include "../yacbnl.h"

Test(metadata, macros) {
  atom_t a[HEADER_OFFSET + 3] = { TYP_NONE, 1, 2, FL_SIGN, 1, 4, 5};

  cr_assert_eq(bna_real_len(a), HEADER_OFFSET + 3);

  atom_t b[HEADER_OFFSET_BIG + 39] = { TYP_BIG, 0, 5, 0, 34, BN_SINF };

  cr_assert_eq(bna_real_len(b), HEADER_OFFSET_BIG + 39);
}

Test(metadata, basic) {
  atom_t* m = make_array_header(TYP_NONE, 0, 0, FL_NONE),
    a[HEADER_OFFSET_BIG] = { TYP_NONE, 0, 0, FL_NONE };

  cr_assert_arr_eq(m, a, sz(HEADER_OFFSET, atom_t));
  free(m);

  m = make_array_header(TYP_NONE, 3, 67, FL_SIGN);
  atom_t b[HEADER_OFFSET] = { TYP_NONE, 3, 67, FL_SIGN };

  cr_assert_arr_eq(m, b, sz(HEADER_OFFSET, atom_t));
  free(m);

  m = make_array_header(TYP_BIG, 300, 670, FL_SIGN);
  atom_t c[HEADER_OFFSET_BIG] = { TYP_BIG, 1, 44, 2, 158, FL_SIGN };

  cr_assert_arr_eq(m, c, sz(HEADER_OFFSET_BIG, atom_t));
  free(m);

  m = make_array_header(TYP_BIG, 65535, 65535, FL_SIGN);
  atom_t d[HEADER_OFFSET_BIG] = { TYP_BIG, 0xFF, 0xFF, 0xFF, 0xFF, FL_SIGN };

  cr_assert_arr_eq(m, d, sz(HEADER_OFFSET_BIG, atom_t));
  free(m);

  m = make_array_header(TYP_BIG | TYP_ZENZ, 65535, 65535, FL_SIGN | FL_NAN);
  atom_t e[HEADER_OFFSET_BIG] = { TYP_BIG | TYP_ZENZ, 0xFF, 0xFF, 0xFF, 0xFF, FL_SIGN | FL_NAN };

  cr_assert_arr_eq(m, e, sz(HEADER_OFFSET_BIG, atom_t));
  free(m);

  m = make_array_header(TYP_NONE, 6, 7, FL_SIGN);
  atom_t f[HEADER_OFFSET] = { TYP_NONE, 6, 7, FL_SIGN };

  cr_assert_arr_eq(m, f, sz(HEADER_OFFSET, atom_t));
  free(m);
}
