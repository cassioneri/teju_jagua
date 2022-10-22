#include "amaru/common.h"
#include "amaru/double.h"

#include <stdbool.h>
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

amaru64_fields_t
amaru_from_double_to_fields(double const value) {

  uint64_t bits;
  memcpy(&bits, &value, sizeof(value));

  // Breakdown into ieee64 fields.

  amaru64_fields_t binary;
  binary.mantissa = AMARU_LSB(bits, mantissa_size);
  bits >>= mantissa_size;
  binary.exponent = AMARU_LSB(bits, exponent_size);

  return binary;
}

amaru64_fields_t
amaru_from_double_to_decimal_compact(double const value) {

  // Conversion to Amaru's binary representation.

  amaru64_fields_t binary        = amaru_from_double_to_fields(value);
  uint32_t         ieee_exponent = binary.exponent;

  binary.exponent += exponent_min;
  if (ieee_exponent != 0) {
    binary.mantissa += AMARU_POW2(uint64_t, mantissa_size);
    binary.exponent -= 1;
  }

  return amaru_ieee64_compact(binary.exponent, binary.mantissa);
}

amaru64_fields_t
amaru_from_double_to_decimal_full(double const value) {

  // Conversion to Amaru's binary representation.

  amaru64_fields_t binary        = amaru_from_double_to_fields(value);
  uint32_t         ieee_exponent = binary.exponent;

  binary.exponent += exponent_min;
  if (ieee_exponent != 0) {
    binary.mantissa += AMARU_POW2(uint64_t, mantissa_size);
    binary.exponent -= 1;
  }

  return amaru_ieee64_full(binary.exponent, binary.mantissa);
}

#ifdef __cplusplus
}
#endif
