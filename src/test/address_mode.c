#include <criterion/criterion.h>

#include "../lib/bn_common.h"

/* addressing mode functions */

Test(addrmodes, 1to2) {
  atom_t h, l;

  samb_u16_to_twoba(2340, &h, &l);
  cr_assert(h == 9 && l == 36);

  samb_u16_to_twoba(65533, &h, &l);
  cr_assert(h == 255 && l == 253);

  samb_u16_to_twoba(65535, &h, &l);
  cr_assert_eq(h, l);
  cr_assert(h == 255);

  samb_u16_to_twoba(256, &h, &l);
  cr_assert(h == 1 && l == 0);

  samb_u16_to_twoba(257, &h, &l);
  cr_assert_eq(h, l);
  cr_assert(h == 1);

  samb_u16_to_twoba(0, &h, &l);
  cr_assert_eq(h, l);
  cr_assert(h == 0);
}

Test(addrmodes, 2to1) {
  cr_assert_eq(2340, samb_twoba_to_u16(9, 36));

  cr_assert_eq(65533, samb_twoba_to_u16(255, 253));

  cr_assert_eq(65535, samb_twoba_to_u16(255, 255));

  cr_assert_eq(256, samb_twoba_to_u16(1, 0));
  cr_assert_eq(257, samb_twoba_to_u16(1, 1));

  cr_assert_eq(0, samb_twoba_to_u16(0, 0));

}
