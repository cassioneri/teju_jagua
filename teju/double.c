// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

#include "teju/common.h"
#include "teju/double.h"

#include <assert.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

teju64_fields_t
teju_double_to_binary(double const value) {

  assert(isfinite(value) && value > 0 && "Invalid double value.");

  typedef teju64_fields_t teju_fields_t;
  typedef teju64_u1_t     teju_u1_t;

  uint32_t const mantissa_width =   53u;
  int32_t  const exponent_min   = -1074;

  teju_u1_t bits;
  memcpy(&bits, &value, sizeof(value));

  teju_u1_t mantissa = teju_lsb(teju_u1_t, bits, mantissa_width - 1u);
  bits >>= (mantissa_width - 1u);

  int32_t exponent = (int32_t) bits;

  if (exponent != 0) {
    exponent -= 1;
    mantissa |= teju_pow2(teju_u1_t, mantissa_width - 1u);
  }

  exponent += exponent_min;
  teju_fields_t binary = {exponent, mantissa};
  return binary;
}

teju64_fields_t
teju_double_to_decimal(double const value) {
  teju64_fields_t binary = teju_double_to_binary(value);
  #if defined(teju_has_uint128)
    return teju_ieee64_with_uint128(binary);
  #else
    return teju_ieee64_no_uint128(binary);
  #endif
}

#ifdef __cplusplus
}
#endif
