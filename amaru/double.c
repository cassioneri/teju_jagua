#include "amaru/common.h"
#include "amaru/double.h"

#include <math.h>
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

static ieee64_t
zero(bool const is_negative) {
  ieee64_t result = { is_negative, 0, 0 };
  return result;
}

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
amaru_from_double_to_decimal_compact(double const value) {

  if (value == 0)
    return zero(signbit(value));

  // Conversion to Amaru's binary representation.

  ieee64_t binary        = amaru_from_double_to_fields(value);
  uint32_t ieee_exponent = binary.exponent;

  binary.exponent += exponent_min;
  if (ieee_exponent != 0) {
    binary.mantissa += AMARU_POW2(uint64_t, mantissa_size);
    binary.exponent -= 1;
  }

  return amaru_binary_to_decimal_ieee64_compact(binary.is_negative,
    binary.exponent, binary.mantissa);
}

ieee64_t
amaru_from_double_to_decimal_full(double const value) {

  if (value == 0)
    return zero(signbit(value));

  // Conversion to Amaru's binary representation.

  ieee64_t binary        = amaru_from_double_to_fields(value);
  uint32_t ieee_exponent = binary.exponent;

  binary.exponent += exponent_min;
  if (ieee_exponent != 0) {
    binary.mantissa += AMARU_POW2(uint64_t, mantissa_size);
    binary.exponent -= 1;
  }

  return amaru_binary_to_decimal_ieee64_full(binary.is_negative,
    binary.exponent, binary.mantissa);
}

uint32_t
amaru_from_double_to_string_compact(double const value, char* str) {
  (void) value, (void) str;
  return 0;
}

uint32_t
amaru_from_double_to_string_full(double const value, char* str) {
  (void) value, (void) str;
  return 0;
}

#ifdef __cplusplus
}
#endif
