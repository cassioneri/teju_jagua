#include "common.h"
#include "double.h"

#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
  mantissa_size =    52,
  exponent_size =    11,
  exponent_min  = -1074
};

ieee64_t
amaru_from_double_to_fields(double const value) {

  uint64_t bits;
  memcpy(&bits, &value, sizeof(value));

  // Breakdown into ieee64 fields.

  ieee64_t binary;
  binary.mantissa = AMARU_LSB(bits, mantissa_size);
  bits >>= mantissa_size;
  binary.exponent = AMARU_LSB(bits, exponent_size);
  bits >>= exponent_size;
  binary.is_negative = bits;

  return binary;
}

ieee64_t
amaru_from_double_to_decimal(double const value) {

  // Conversion to Amaru's binary representation.

  ieee64_t binary        = amaru_from_double_to_fields(value);
  uint32_t ieee_exponent = binary.exponent;

  binary.exponent += exponent_min;
  if (ieee_exponent != 0) {
    binary.mantissa += AMARU_POW2(uint64_t, mantissa_size);
    binary.exponent -= 1;
  }

  return amaru_binary_to_decimal_ieee64(binary.is_negative, binary.exponent,
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
