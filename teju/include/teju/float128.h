// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

/**
 * @file teju/src/float128.h
 *
 * Tejú Jaguá and helpers for float128_t values.
 */

#ifndef TEJU_TEJU_INCLUDE_TEJU_FLOAT128_H_
#define TEJU_TEJU_INCLUDE_TEJU_FLOAT128_H_

#if defined(teju_has_float128)

#include "teju/src/common.h"
#include "teju/src/config.h"

#include "teju/src/generated/ieee128.h"

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
teju128_fields_t
teju_float128_to_binary(float128_t const value) {

  assert(isfinite(value) && value > 0 && "Invalid float128_t value.");

  typedef teju128_fields_t teju_fields_t;
  typedef teju128_u1_t     teju_u1_t;

  uint32_t const mantissa_width =    113u;
  int32_t  const exponent_min   = -16494;

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
  teju128_fields_t binary = {exponent, mantissa};
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
teju128_fields_t
teju_float128_to_decimal(float128_t const value) {
  teju128_fields_t binary = teju_float128_to_binary(value);
  return teju_ieee128(binary);
}

#ifdef __cplusplus
}
#endif

#endif // defined(teju_has_float128)
#endif // TEJU_TEJU_INCLUDE_TEJU_FLOAT128_H_
