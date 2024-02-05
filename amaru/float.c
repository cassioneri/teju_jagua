#include "amaru/common.h"
#include "amaru/float.h"
#include "amaru/ieee754.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
  exponent_size = amaru_ieee754_binary32_exponent_size,
  mantissa_size = amaru_ieee754_binary32_mantissa_size,
  exponent_min  = amaru_min_binary_exponent_from_ieee754(exponent_size,
    mantissa_size)
};

amaru32_fields_t
amaru_float_to_ieee32(float const value) {

  uint32_t bits;
  memcpy(&bits, &value, sizeof(value));

  amaru32_fields_t binary;
  binary.mantissa = amaru_lsb(bits, mantissa_size);
  bits >>= mantissa_size;
  binary.exponent = (int32_t) amaru_lsb(bits, exponent_size);

  return binary;
}

amaru32_fields_t
amaru_ieee32_to_amaru_binary(amaru32_fields_t ieee32) {

  int32_t  e = ieee32.exponent + exponent_min;
  uint32_t m = ieee32.mantissa;

  if (ieee32.exponent != 0) {
    e -= 1;
    m += amaru_pow2(uint64_t, mantissa_size);
  }

  amaru32_fields_t amaru_binary = { e, m };
  return amaru_binary;
}

amaru32_fields_t
amaru_float_to_amaru_decimal(float const value) {
  amaru32_fields_t ieee32       = amaru_float_to_ieee32(value);
  amaru32_fields_t amaru_binary = amaru_ieee32_to_amaru_binary(ieee32);
  return amaru_ieee32(amaru_binary);
}

#ifdef __cplusplus
}
#endif
