#include "amaru/common.h"
#include "amaru/float.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
  mantissa_size =   23,
  exponent_size =    8,
  exponent_min  = -149
};

amaru32_fields_t
amaru_to_ieee32_fields(float const value) {

  uint32_t bits;
  memcpy(&bits, &value, sizeof(value));

  // Breakdown into ieee32 fields.

  amaru32_fields_t binary;
  binary.mantissa = amaru_lsb(bits, mantissa_size);
  bits >>= mantissa_size;
  binary.exponent = amaru_lsb(bits, exponent_size);

  return binary;
}

amaru32_fields_t
amaru_from_float_to_decimal_compact(float const value) {

  // Conversion to Amaru's binary representation.

  amaru32_fields_t binary        = amaru_to_ieee32_fields(value);
  uint32_t         ieee_exponent = binary.exponent;

  binary.exponent += exponent_min;
  if (ieee_exponent != 0) {
    binary.mantissa += amaru_pow2(uint32_t, mantissa_size);
    binary.exponent -= 1;
  }

  return amaru_ieee32_compact(binary.exponent, binary.mantissa);
}

amaru32_fields_t
amaru_from_float_to_decimal_full(float const value) {

  // Conversion to Amaru's binary representation.

  amaru32_fields_t binary        = amaru_to_ieee32_fields(value);
  uint32_t         ieee_exponent = binary.exponent;

  binary.exponent += exponent_min;
  if (ieee_exponent != 0) {
    binary.mantissa += amaru_pow2(uint32_t, mantissa_size);
    binary.exponent -= 1;
  }

  return amaru_ieee32_full(binary.exponent, binary.mantissa);
}

#ifdef __cplusplus
}
#endif
