// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2024 Cassio Neri <cassio.neri@gmail.com>

#include "amaru/common.h"
#include "amaru/double.h"
#include "amaru/ieee754.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
  exponent_size = amaru_ieee754_binary64_exponent_size,
  mantissa_size = amaru_ieee754_binary64_mantissa_size,
  exponent_min  = amaru_min_binary_exponent_from_ieee754(exponent_size,
    mantissa_size)
};

amaru64_fields_t
amaru_double_to_ieee64(double const value) {

  uint64_t bits;
  memcpy(&bits, &value, sizeof(value));

  amaru64_fields_t binary;
  binary.mantissa = amaru_lsb(bits, mantissa_size);
  bits >>= mantissa_size;
  binary.exponent = (int32_t)amaru_lsb(bits, exponent_size);

  return binary;
}

amaru64_fields_t
amaru_ieee64_to_amaru_binary(amaru64_fields_t const ieee64) {

  int32_t  e = ieee64.exponent + exponent_min;
  uint64_t m = ieee64.mantissa;

  if (ieee64.exponent != 0) {
    e -= 1;
    m += amaru_pow2(uint64_t, mantissa_size);
  }

  amaru64_fields_t amaru_binary = { e, m };
  return amaru_binary;
}

amaru64_fields_t
amaru_double_to_amaru_decimal(double const value) {
  amaru64_fields_t ieee64       = amaru_double_to_ieee64(value);
  amaru64_fields_t amaru_binary = amaru_ieee64_to_amaru_binary(ieee64);
  #if defined(AMARU_HAS_UINT128)
    return amaru_ieee64_with_uint128(amaru_binary);
  #else
    return amaru_ieee64_no_uint128(amaru_binary);
  #endif
}

#ifdef __cplusplus
}
#endif
