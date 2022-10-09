#include "common.h"
#include "double.h"

#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

ieee64_t
amaru_from_double_to_decimal(double const value) {

  uint32_t const mantissa_size = 52;
  uint32_t const exponent_size = 11;
  int32_t  const exponent_min  = -1074;

  uint64_t bits;
  memcpy(&bits, &value, sizeof(value));

  // Breakdown into ieee64 fields.

  ieee64_t ieee;
  ieee.mantissa = AMARU_LSB(bits, mantissa_size);
  bits >>= mantissa_size;
  ieee.exponent = AMARU_LSB(bits, exponent_size);
  bits >>= exponent_size;
  ieee.is_negative = bits;

  // Conversion to Amaru's binary representation.

  ieee64_t binary = ieee;
  binary.exponent += exponent_min;
  if (ieee.exponent != 0) {
    binary.mantissa += AMARU_POW2(uint64_t, mantissa_size);
    binary.exponent -= 1;
  }

  return amaru_bin_to_dec_ieee64(binary.is_negative, binary.exponent,
    binary.mantissa);
}

uint32_t
amaru_from_double_to_string(double const value, char* str) {
  (void) value, (void) str;
  return 0;
}


#ifdef __cplusplus
}
#endif
