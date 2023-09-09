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
amaru_to_ieee128(float128_t const value) {

  uint128_t bits;
  memcpy(&bits, &value, sizeof(value));

  amaru128_fields_t binary;
  binary.mantissa = amaru_lsb(bits, mantissa_size);
  bits >>= mantissa_size;
  binary.exponent = amaru_lsb(bits, exponent_size);

  return binary;
}

amaru128_fields_t
amaru_ieee128_to_amaru(amaru128_fields_t ieee128) {

  amaru128_fields_t amaru_binary = ieee128;

  amaru_binary.exponent += exponent_min;
  if (ieee128.exponent != 0) {
    amaru_binary.mantissa += amaru_pow2(uint128_t, mantissa_size);
    amaru_binary.exponent -= 1;
  }

  return amaru_binary;
}

amaru128_fields_t
amaru_float128_compact(float128_t const value) {
  amaru128_fields_t ieee128      = amaru_to_ieee128(value);
  amaru128_fields_t amaru_binary = amaru_ieee128_to_amaru(ieee128);
  return amaru_ieee128_compact(amaru_binary);
}

amaru128_fields_t
amaru_float128_full(float128_t const value) {
  amaru128_fields_t ieee128      = amaru_to_ieee128(value);
  amaru128_fields_t amaru_binary = amaru_ieee128_to_amaru(ieee128);
  return amaru_ieee128_full(amaru_binary);
}

#ifdef __cplusplus
}
#endif

# endif // defined(__clang__) || defined(__GNUC__)
