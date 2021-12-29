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
ieee32_t to_amaru_bin_float(float const value) {

  uint32_t const mantissa_size = 23;
  uint32_t const exponent_size = 8;
  int32_t  const exponent_min  = -149;

  uint32_t i;
  memcpy(&i, &value, sizeof(value));

  ieee32_t ieee;
  ieee.mantissa = AMARU_LSB(i, mantissa_size);
  i >>= mantissa_size;
  ieee.exponent = AMARU_LSB(i, exponent_size);
  i >>= exponent_size;
  ieee.negative = i;

  ieee32_t amaru = ieee;
  amaru.exponent += exponent_min;
  if (ieee.exponent != 0) {
    amaru.mantissa += AMARU_POW2(uint32_t, mantissa_size);
    amaru.exponent -= 1;
  }

  return amaru;
}

static inline
ieee32_t to_amaru_dec_float(float const value) {
  ieee32_t const amaru = to_amaru_bin_float(value);
  return to_amaru_dec_ieee32(amaru.negative, amaru.exponent, amaru.mantissa);
}

static inline
ieee64_t to_amaru_bin_double(double const value) {

  uint32_t const mantissa_size = 52;
  uint32_t const exponent_size = 11;
  int32_t  const exponent_min  = -1074;

  uint64_t i;
  memcpy(&i, &value, sizeof(value));

  ieee64_t ieee;
  ieee.mantissa = AMARU_LSB(i, mantissa_size);
  i >>= mantissa_size;
  ieee.exponent = AMARU_LSB(i, exponent_size);
  i >>= exponent_size;
  ieee.negative = i;

  ieee64_t amaru = ieee;
  amaru.exponent += exponent_min;
  if (ieee.exponent != 0) {
    amaru.mantissa += AMARU_POW2(uint64_t, mantissa_size);
    amaru.exponent -= 1;
  }

  return amaru;
}

static inline
ieee64_t to_amaru_dec_double(double const value) {
  ieee64_t const amaru = to_amaru_bin_double(value);
  return to_amaru_dec_ieee64(amaru.negative, amaru.exponent, amaru.mantissa);
}

#ifdef __cplusplus
}
#endif
