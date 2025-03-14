// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

/**
 * @file teju/float.h
 *
 * Teju Jagua and helpers for float values.
 */

#ifndef TEJU_TEJU_FLOAT_H_
#define TEJU_TEJU_FLOAT_H_

#if defined(teju_has_uint128)
  #include "teju/generated/ieee32_with_uint128.h"
#else
  #include "teju/generated/ieee32_no_uint128.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Gets the binary representation of a given value.
 *
 * @pre isfinite(value) && value > 0.
 *
 * @param value             The given value.
 *
 * @returns The binary representation of the given value.
 */
teju32_fields_t
teju_float_to_binary(float value);

/**
 * @brief Gets the decimal representation of a given value.
 *
 * @pre isfinite(value) && value > 0.
 *
 * @param value             The given value.
 *
 * @returns The decimal representation of the given value.
 */
teju32_fields_t
teju_float_to_decimal(float value);

#ifdef __cplusplus
}
#endif

#endif // TEJU_TEJU_FLOAT_H_
