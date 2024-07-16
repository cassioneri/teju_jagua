// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2024 Cassio Neri <cassio.neri@gmail.com>

/**
 * @file teju/float128.h
 *
 * Teju Jagua and helpers for float128_t values.
 */

#ifndef TEJU_TEJU_FLOAT128_H_
#define TEJU_TEJU_FLOAT128_H_

#include "teju/config.h"

#if defined(teju_has_float128)

#include "teju/generated/ieee128.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Gets IEEE-754's binary128 representation of a float128_t.
 *
 * See https://en.wikipedia.org/wiki/Quadruple-precision_floating-point_format
 *
 * @pre value > 0.
 *
 * @param value             The given float128_t.
 *
  * @returns IEEE-754's binary128 representation of value.
 */
teju128_fields_t
teju_float128_to_ieee128(float128_t value);

/**
 * @brief Gets Teju Jagua's binary representation of a given IEEE-754 binary128
 * one.
 *
 * @pre value > 0.
 *
 * @param ieee128           The given IEEE-754 binary128 representation.
 *
 * @returns Teju Jagua's binary representation value.
 */
teju128_fields_t
teju_ieee128_to_binary(teju128_fields_t ieee128);

/**
 * @brief Gets Teju Jagua's decimal representation of a float128_t.
 *
 * @pre value > 0.
 *
 * @param value             The given float128_t.
 *
 * @returns Teju Jagua's decimal representation of value.
 */
teju128_fields_t
teju_float128_to_decimal(float128_t value);

#ifdef __cplusplus
}
#endif

#endif // defined(teju_has_float128)
#endif // TEJU_TEJU_FLOAT128_H_
