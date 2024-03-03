// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2024 Cassio Neri <cassio.neri@gmail.com>

#include "teju/common.h"
#include "teju/double.h"
#include "teju/ieee754.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
  exponent_size = teju_ieee754_binary64_exponent_size,
  mantissa_size = teju_ieee754_binary64_mantissa_size,
  exponent_min  = teju_min_binary_exponent_from_ieee754(exponent_size,
    mantissa_size)
};

teju64_fields_t
teju_double_to_ieee64(double const value) {

  uint64_t bits;
  memcpy(&bits, &value, sizeof(value));

  teju64_fields_t binary;
  binary.mantissa = teju_lsb(bits, mantissa_size);
  bits >>= mantissa_size;
  binary.exponent = (int32_t) teju_lsb(bits, exponent_size);

  return binary;
}

teju64_fields_t
teju_ieee64_to_teju_binary(teju64_fields_t const ieee64) {

  int32_t  e = ieee64.exponent + exponent_min;
  uint64_t m = ieee64.mantissa;

  if (ieee64.exponent != 0) {
    e -= 1;
    m += teju_pow2(uint64_t, mantissa_size);
  }

  teju64_fields_t teju_binary = { e, m };
  return teju_binary;
}

teju64_fields_t
teju_double_to_teju_decimal(double const value) {
  teju64_fields_t ieee64      = teju_double_to_ieee64(value);
  teju64_fields_t teju_binary = teju_ieee64_to_teju_binary(ieee64);
  #if defined(teju_has_uint128)
    return teju_ieee64_with_uint128(teju_binary);
  #else
    return teju_ieee64_no_uint128(teju_binary);
  #endif
}

#ifdef __cplusplus
}
#endif
