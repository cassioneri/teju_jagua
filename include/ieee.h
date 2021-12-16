#pragma once

#include "common.h"
#include <stdint.h>
#include <string.h>

#include "generated/ieee32.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline
ieee32_t amaru_float(float const value) {

  uint32_t const mantissa_size = 23;
  uint32_t const exponent_size = 8;
  int32_t  const exponent_min  = -149;

  uint32_t i;
  memcpy(&i, &value, sizeof(value));

  ieee32_t ieee;
  ieee.mantissa = AMARU_LOWER_BITS(i, mantissa_size);
  i >>= mantissa_size;
  ieee.exponent = AMARU_LOWER_BITS(i, exponent_size);
  i >>= exponent_size;
  ieee.negative = i;

  ieee.mantissa += ieee.exponent == 0 ? 0 : AMARU_POW2(uint32_t, mantissa_size);
  ieee.exponent  = exponent_min + (ieee.exponent == 0 ? 0 : ieee.exponent - 1);

  return amaru_ieee32(ieee);
}

#ifdef __cplusplus
}
#endif
