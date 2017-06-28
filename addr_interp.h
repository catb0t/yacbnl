#ifndef ADDR_INTERP_H
#define ADDR_INTERP_H

#include "bn_common.h"

void u16_to_twoba (const uint16_t n, atom_t* const ah, atom_t* const al) {
  *ah = (atom_t) (n >> (atom_t) 8);// high bits
  *al = (atom_t) (n &  (atom_t) 0xFF);     // low 8 bits
}

uint16_t twoba_to_u16 (const atom_t ah, const atom_t al) {
  // put the high 8 bits at the top and the rest at the bottom
  return (uint16_t) ( (ah << 8) | (atom_t) al);
}

/*atom_t* u64_to_octba (const uint64_t n) {

  return NULL;
}

uint64_t octba_to_u64 (const atom_t* const bytes) {
  uint64_t result = 0;

  for (size_t i = 0; i < CHAR_BIT; i++) {
    result |= (uint64_t) bytes[i] << ( (CHAR_BIT - (i + 1) ) * CHAR_BIT );
  }

  return result;
}
*/
#endif /* end of include guard: ADDR_INTERP_H */
