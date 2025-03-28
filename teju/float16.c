// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

#include "teju/config.h"

#if defined(teju_has_float16)

#include "teju/common.h"
#include "teju/float16.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

teju32_fields_t
teju_float16_to_binary(float16_t const value) {

  typedef teju32_fields_t teju_fields_t;
  typedef teju32_u1_t     teju_u1_t;

  uint32_t const mantissa_size =  11u;
  int32_t  const exponent_min  = -24;

  teju_u1_t bits;
  memcpy(&bits, &value, sizeof(value));

  teju_u1_t mantissa = teju_lsb(bits, mantissa_size - 1u);
  bits >>= (mantissa_size - 1u);

  int32_t exponent = (int32_t) bits;

  if (exponent != 0) {
    exponent -= 1;
    mantissa |= teju_pow2(teju_u1_t, mantissa_size - 1u);
  }

  exponent += exponent_min;
  teju32_fields_t binary = {mantissa, exponent};
  return binary;
}

teju32_fields_t
teju_float16_to_decimal(float16_t const value) {
  teju32_fields_t binary = teju_float16_to_binary(value);
  return teju_ieee16(binary);
}

#ifdef __cplusplus
}
#endif

# endif // defined(teju_has_float16)
