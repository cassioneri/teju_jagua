#if defined(__clang__) || defined(__GNUC__)

#include "amaru/common.h"
#include "amaru/float128.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
  mantissa_size =    112,
  exponent_size =     15,
  exponent_min  = -16494
};

amaru128_fields_t
amaru_to_ieee128_fields(__float128 const value) {

  __uint128_t bits;
  memcpy(&bits, &value, sizeof(value));

  // Breakdown into ieee128 fields.

  amaru128_fields_t binary;
  binary.mantissa = amaru_lsb(bits, mantissa_size);
  bits >>= mantissa_size;
  binary.exponent = amaru_lsb(bits, exponent_size);

  return binary;
}

amaru128_fields_t
amaru_from_float128_to_decimal_compact(__float128 const value) {

  // Conversion to Amaru's binary representation.

  amaru128_fields_t binary       = amaru_to_ieee128_fields(value);
  uint32_t         ieee_exponent = binary.exponent;

  binary.exponent += exponent_min;
  if (ieee_exponent != 0) {
    binary.mantissa += amaru_pow2(__uint128_t, mantissa_size);
    binary.exponent -= 1;
  }

  return amaru_ieee128_compact(binary.exponent, binary.mantissa);
}

amaru128_fields_t
amaru_from_float128_to_decimal_full(__float128 const value) {

  // Conversion to Amaru's binary representation.

  amaru128_fields_t binary        = amaru_to_ieee128_fields(value);
  uint32_t         ieee_exponent = binary.exponent;

  binary.exponent += exponent_min;
  if (ieee_exponent != 0) {
    binary.mantissa += amaru_pow2(__uint128_t, mantissa_size);
    binary.exponent -= 1;
  }

  return amaru_ieee128_full(binary.exponent, binary.mantissa);
}

#ifdef __cplusplus
}
#endif

# endif // defined(__clang__) || defined(__GNUC__)
