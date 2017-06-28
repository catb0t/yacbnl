#include <criterion/criterion.h>

#include "../addr_interp.h"

/* addressing mode functions */

Test(addrmodes, 1to2) {
  atom_t h, l;

  u16_to_twoba(2340, &h, &l);
  cr_assert(h == 9 && l == 36);

  u16_to_twoba(65533, &h, &l);
  cr_assert(h == 255 && l == 253);

  u16_to_twoba(65535, &h, &l);
  cr_assert_eq(h, l);
  cr_assert(h == 255);

  u16_to_twoba(256, &h, &l);
  cr_assert(h == 1 && l == 0);

  u16_to_twoba(257, &h, &l);
  cr_assert_eq(h, l);
  cr_assert(h == 1);

  u16_to_twoba(0, &h, &l);
  cr_assert_eq(h, l);
  cr_assert(h == 0);
}

Test(addrmodes, 2to1) {
  cr_assert_eq(2340, twoba_to_u16(9, 36));

  cr_assert_eq(65533, twoba_to_u16(255, 253));

  cr_assert_eq(65535, twoba_to_u16(255, 255));

  cr_assert_eq(256, twoba_to_u16(1, 0));
  cr_assert_eq(257, twoba_to_u16(1, 1));

  cr_assert_eq(0, twoba_to_u16(0, 0));

}
