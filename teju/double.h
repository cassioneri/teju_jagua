// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

/**
 * @file teju/double.h
 *
 * Teju Jagua and helpers for double values.
 */

#ifndef TEJU_TEJU_DOUBLE_H_
#define TEJU_TEJU_DOUBLE_H_

#if defined(teju_has_uint128)
  #include "teju/generated/ieee64_with_uint128.h"
#else
  #include "teju/generated/ieee64_no_uint128.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Gets IEEE-754's binary64 representation of a double.
 *
 * See https://en.wikipedia.org/wiki/Double-precision_floating-point_format
 *
 * @pre value > 0.
 *
 * @param value             The given double.
 *
  * @returns IEEE-754's binary64 representation of value.
 */
teju64_fields_t
teju_double_to_ieee64(double value);

/**
 * @brief Gets Teju Jagua's binary representation of a given IEEE-754 binary64
 * one.
 *
 * @pre value > 0.
 *
 * @param ieee64            The given IEEE-754 binary64 representation.
 *
 * @returns Teju Jagua's binary representation value.
 */
teju64_fields_t
teju_ieee64_to_binary(teju64_fields_t ieee64);

/**
 * @brief Gets Teju Jagua's decimal representation of a double.
 *
 * @pre value > 0.
 *
 * @param value             The given double.
 *
 * @returns Teju Jagua's decimal representation of value.
 */
teju64_fields_t
teju_double_to_decimal(double value);

#ifdef __cplusplus
}
#endif

#endif // TEJU_TEJU_DOUBLE_H_
