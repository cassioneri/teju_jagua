#include "amaru/config.h"

#if defined(AMARU_HAS_FLOAT128)

#include "amaru/common.h"
#include "amaru/float128.h"
#include "amaru/ieee754.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
  exponent_size = amaru_ieee754_binary128_exponent_size,
  mantissa_size = amaru_ieee754_binary128_mantissa_size,
  exponent_min  = amaru_min_binary_exponent_from_ieee754(exponent_size,
    mantissa_size)
};

amaru128_fields_t
amaru_float128_to_ieee128(float128_t const value) {

  uint128_t bits;
  memcpy(&bits, &value, sizeof(value));

  amaru128_fields_t binary;
  binary.mantissa = amaru_lsb(bits, mantissa_size);
  bits >>= mantissa_size;
  binary.exponent = amaru_lsb(bits, exponent_size);

  return binary;
}

amaru128_fields_t
amaru_ieee128_to_amaru_binary(amaru128_fields_t ieee128) {

  amaru128_fields_t amaru_binary = ieee128;

  amaru_binary.exponent += exponent_min;
  if (ieee128.exponent != 0) {
    amaru_binary.mantissa += amaru_pow2(uint128_t, mantissa_size);
    amaru_binary.exponent -= 1;
  }

  return amaru_binary;
}

amaru128_fields_t
amaru_float128_to_amaru_decimal_compact(float128_t const value) {
  amaru128_fields_t ieee128      = amaru_float128_to_ieee128(value);
  amaru128_fields_t amaru_binary = amaru_ieee128_to_amaru_binary(ieee128);
  return amaru_ieee128_compact(amaru_binary);
}

amaru128_fields_t
amaru_float128_to_amaru_decimal_full(float128_t const value) {
  amaru128_fields_t ieee128      = amaru_float128_to_ieee128(value);
  amaru128_fields_t amaru_binary = amaru_ieee128_to_amaru_binary(ieee128);
  return amaru_ieee128_full(amaru_binary);
}

#ifdef __cplusplus
}
#endif

# endif // defined(AMARU_HAS_FLOAT128)
