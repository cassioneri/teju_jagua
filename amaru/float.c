#include "amaru/common.h"
#include "amaru/float.h"

#include <math.h>
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

static amaru32_fields_t
zero(bool const is_negative) {
  amaru32_fields_t result = { is_negative, 0, 0 };
  return result;
}

amaru32_fields_t
amaru_from_float_to_fields(float const value) {

  uint32_t bits;
  memcpy(&bits, &value, sizeof(value));

  // Breakdown into ieee32 fields.

  amaru32_fields_t binary;
  binary.mantissa = AMARU_LSB(bits, mantissa_size);
  bits >>= mantissa_size;
  binary.exponent = AMARU_LSB(bits, exponent_size);
  bits >>= exponent_size;
  binary.is_negative = bits;

  return binary;
}

amaru32_fields_t
amaru_from_float_to_decimal_compact(float const value) {

  if (value == 0)
    return zero(signbit(value));

  // Conversion to Amaru's binary representation.

  amaru32_fields_t binary        = amaru_from_float_to_fields(value);
  uint32_t         ieee_exponent = binary.exponent;

  binary.exponent += exponent_min;
  if (ieee_exponent != 0) {
    binary.mantissa += AMARU_POW2(uint32_t, mantissa_size);
    binary.exponent -= 1;
  }

  return amaru_binary_to_decimal_ieee32_compact(binary.is_negative,
    binary.exponent, binary.mantissa);
}

amaru32_fields_t
amaru_from_float_to_decimal_full(float const value) {

  if (value == 0)
    return zero(signbit(value));

  // Conversion to Amaru's binary representation.

  amaru32_fields_t binary        = amaru_from_float_to_fields(value);
  uint32_t         ieee_exponent = binary.exponent;

  binary.exponent += exponent_min;
  if (ieee_exponent != 0) {
    binary.mantissa += AMARU_POW2(uint32_t, mantissa_size);
    binary.exponent -= 1;
  }

  return amaru_binary_to_decimal_ieee32_full(binary.is_negative,
    binary.exponent, binary.mantissa);
}

uint32_t
amaru_from_float_to_string_compact(float const value, char* str) {
  (void) value, (void) str;
  return 0;
}

uint32_t
amaru_from_float_to_string_full(float const value, char* str) {
  (void) value, (void) str;
  return 0;
}

#ifdef __cplusplus
}
#endif
