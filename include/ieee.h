#pragma once

#include "common.h"
#include <stdint.h>
#include <string.h>

#include "generated/ieee32.h"
#include "generated/ieee64.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline
ieee32_t to_ieee32(float const value) {

  uint32_t const mantissa_size = 23;
  uint32_t const exponent_size = 8;

  uint32_t i;
  memcpy(&i, &value, sizeof(value));

  ieee32_t ieee;
  ieee.mantissa = AMARU_LOWER_BITS(i, mantissa_size);
  i >>= mantissa_size;
  ieee.exponent = AMARU_LOWER_BITS(i, exponent_size);
  i >>= exponent_size;
  ieee.negative = i;

  return ieee;
}

static inline
ieee32_t amaru_float(float const value) {

  uint32_t const mantissa_size = 23;
  int32_t  const exponent_min  = -149;

  // Fill in as ieee32 fields...
  ieee32_t amaru = to_ieee32(value);
  // ...and then changes to Amaru's representation.
  amaru.mantissa += amaru.exponent == 0 ? 0 : AMARU_POW2(uint32_t, mantissa_size);
  amaru.exponent  = exponent_min + (amaru.exponent == 0 ? 0 : amaru.exponent - 1);

  return amaru_ieee32(amaru);
}

static inline
ieee64_t to_ieee64(double const value) {

  uint32_t const mantissa_size = 52;
  uint32_t const exponent_size = 11;

  uint64_t i;
  memcpy(&i, &value, sizeof(value));

  ieee64_t ieee;
  ieee.mantissa = AMARU_LOWER_BITS(i, mantissa_size);
  i >>= mantissa_size;
  ieee.exponent = AMARU_LOWER_BITS(i, exponent_size);
  i >>= exponent_size;
  ieee.negative = i;

  return ieee;
}

static inline
ieee64_t amaru_double(double const value) {

  uint32_t const mantissa_size = 52;
  int32_t  const exponent_min  = -1074;

  // Fill in as ieee64 fields...
  ieee64_t amaru = to_ieee64(value);
  // ...and then changes to Amaru's representation.
  amaru.mantissa += amaru.exponent == 0 ? 0 : AMARU_POW2(uint64_t, mantissa_size);
  amaru.exponent  = exponent_min + (amaru.exponent == 0 ? 0 : amaru.exponent - 1);

  return amaru_ieee64(amaru);
}

#ifdef __cplusplus
}
#endif
