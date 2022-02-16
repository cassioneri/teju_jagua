#include "../generated/ieee32.h"
#include "../include/common.h"
#include "../include/ieee32_conv.h"

#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t amaru_val_to_str_ieee32(float const val, char* str) {
  (void) val, (void) str;
  return 0;
}

ieee32_t amaru_val_to_dec_ieee32(float const val) {

  uint32_t const mantissa_size = 23;
  uint32_t const exponent_size = 8;
  int32_t  const exponent_min  = -149;

  uint32_t bits;
  memcpy(&bits, &val, sizeof(val));

  // Breakdown into ieee32 fields.

  ieee32_t ieee;
  ieee.mantissa = AMARU_LSB(bits, mantissa_size);
  bits >>= mantissa_size;
  ieee.exponent = AMARU_LSB(bits, exponent_size);
  bits >>= exponent_size;
  ieee.negative = bits;

  // Conversion to Amaru's binary representation.

  ieee32_t binary = ieee;
  binary.exponent += exponent_min;
  if (ieee.exponent != 0) {
    binary.mantissa += AMARU_POW2(uint32_t, mantissa_size);
    binary.exponent -= 1;
  }

  return amaru_bin_to_dec_ieee32(binary.negative, binary.exponent,
    binary.mantissa);
}

#ifdef __cplusplus
}
#endif
