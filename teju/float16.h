// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

/**
 * @file teju/float16.h
 *
 * Teju Jagua and helpers for float16_t values.
 */

#ifndef TEJU_TEJU_FLOAT16_H_
#define TEJU_TEJU_FLOAT16_H_

#include "teju/config.h"

#if defined(teju_has_float16)

#include "teju/generated/ieee16.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Gets IEEE-754's binary16 representation of a float16_t.
 *
 * See https://en.wikipedia.org/wiki/Half-precision_floating-point_format
 *
 * @pre value > 0.
 *
 * @param value             The given float16_t.
 *
  * @returns IEEE-754's binary16 representation of value.
 */
teju16_fields_t
teju_float16_to_ieee16(float16_t value);

/**
 * @brief Gets Teju Jagua's binary representation of a given IEEE-754 binary16
 * one.
 *
 * @pre value > 0.
 *
 * @param ieee16            The given IEEE-754 binary16 representation.
 *
 * @returns Teju Jagua's binary representation value.
 */
teju16_fields_t
teju_ieee16_to_binary(teju16_fields_t ieee16);

/**
 * @brief Gets Teju Jagua's decimal representation of a float16_t.
 *
 * @pre value > 0.
 *
 * @param value             The given float16_t.
 *
 * @returns Teju Jagua's decimal representation of value.
 */
teju16_fields_t
teju_float16_to_decimal(float16_t value);

#ifdef __cplusplus
}
#endif

#endif // defined(teju_has_float16)
#endif // TEJU_TEJU_FLOAT16_H_
