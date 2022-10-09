#include "common.h"
#include "float.h"

#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

ieee32_t
amaru_from_float_to_decimal(float const value) {

  uint32_t const mantissa_size = 23;
  uint32_t const exponent_size = 8;
  int32_t  const exponent_min  = -149;

  uint32_t bits;
  memcpy(&bits, &value, sizeof(value));

  // Breakdown into ieee32 fields.

  ieee32_t ieee;
  ieee.mantissa = AMARU_LSB(bits, mantissa_size);
  bits >>= mantissa_size;
  ieee.exponent = AMARU_LSB(bits, exponent_size);
  bits >>= exponent_size;
  ieee.is_negative = bits;

  // Conversion to Amaru's binary representation.

  ieee32_t binary = ieee;
  binary.exponent += exponent_min;
  if (ieee.exponent != 0) {
    binary.mantissa += AMARU_POW2(uint32_t, mantissa_size);
    binary.exponent -= 1;
  }

  return amaru_bin_to_dec_ieee32(binary.is_negative, binary.exponent,
    binary.mantissa);
}

uint32_t
amaru_from_float_to_string(float const value, char* str) {
  (void) value, (void) str;
  return 0;
}

#ifdef __cplusplus
}
#endif
