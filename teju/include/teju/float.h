// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

/**
 * @file teju/float.h
 *
 * Tejú Jaguá and helpers for float values.
 */

#ifndef TEJU_TEJU_INCLUDE_TEJU_FLOAT_H_
#define TEJU_TEJU_INCLUDE_TEJU_FLOAT_H_

#include "teju/src/common.h"
#include "teju/src/config.h"

#if defined(teju_has_uint128)
  #include "teju/src/generated/ieee32_with_uint128.h"
#else
  #include "teju/src/generated/ieee32_no_uint128.h"
#endif

#include <assert.h>
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Gets the binary representation of a given value.
 *
 * @param  value            The given value.
 *
 * @pre isfinite(value) && value > 0.
 *
 * @returns The binary representation of the given value.
 */
inline
teju32_fields_t
teju_float_to_binary(float const value) {

  assert(isfinite(value) && value > 0 && "Invalid float value.");

  typedef teju32_fields_t teju_fields_t;
  typedef teju32_u1_t     teju_u1_t;

  uint32_t const mantissa_width =  24u;
  int32_t  const exponent_min   = -149;

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

/**
 * @brief Gets the decimal representation of a given value.
 *
 * @param  value            The given value.
 *
 * @pre isfinite(value) && value > 0.
 *
 * @returns The decimal representation of the given value.
 */
inline
teju32_fields_t
teju_float_to_decimal(float const value) {
  teju32_fields_t binary = teju_float_to_binary(value);
  #if defined(teju_has_uint128)
    return teju_ieee32_with_uint128(binary);
  #else
    return teju_ieee32_no_uint128(binary);
  #endif
}

#ifdef __cplusplus
}
#endif

#endif // TEJU_TEJU_INCLUDE_TEJU_FLOAT_H_
