// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2024 Cassio Neri <cassio.neri@gmail.com>

#include "teju/config.h"

#if defined(teju_has_float128)

#include "teju/common.h"
#include "teju/float128.h"
#include "teju/ieee754.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

// Conversion from IEEE-754's parameters to Teju Jagua's.
enum {
  exponent_size = teju_ieee754_binary128_exponent_size,
  mantissa_size = teju_ieee754_binary128_mantissa_size,
  exponent_min  = teju_ieee754_binary128_exponent_min - mantissa_size,
};

teju128_fields_t
teju_float128_to_ieee128(float128_t const value) {

  uint128_t bits;
  memcpy(&bits, &value, sizeof(value));

  teju128_fields_t binary;
  binary.mantissa = teju_lsb(bits, mantissa_size);
  bits >>= mantissa_size;
  binary.exponent = (int32_t) teju_lsb(bits, exponent_size);

  return binary;
}

teju128_fields_t
teju_ieee128_to_binary(teju128_fields_t ieee128) {

  int32_t   e = ieee128.exponent + exponent_min;
  uint128_t m = ieee128.mantissa;

  if (ieee128.exponent != 0) {
    e -= 1;
    m += teju_pow2(uint128_t, mantissa_size);
  }

  teju128_fields_t teju_binary = { e, m };
  return teju_binary;
}

teju128_fields_t
teju_float128_to_decimal(float128_t const value) {
  teju128_fields_t ieee128     = teju_float128_to_ieee128(value);
  teju128_fields_t teju_binary = teju_ieee128_to_binary(ieee128);
  return teju_ieee128(teju_binary);
}

#ifdef __cplusplus
}
#endif

# endif // defined(teju_has_float128)
