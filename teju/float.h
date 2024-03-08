// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2024 Cassio Neri <cassio.neri@gmail.com>

/**
 * @file teju/float.h
 *
 * Teju Jagua and helpers for float values.
 */

#ifndef TEJU_TEJU_FLOAT_H_
#define TEJU_TEJU_FLOAT_H_

#include "teju/generated/ieee32.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Gets IEEE-754's binary32 representation of a float.
 *
 * See https://en.wikipedia.org/wiki/Single-precision_floating-point_format
 *
 * @pre value > 0.
 *
 * @param value             The given float.
 *
 * @returns IEEE-754's binary32 representation of value.
 */
teju32_fields_t
teju_float_to_ieee32(float value);

/**
 * @brief Gets teju's binary representation of a given IEEE-754 binary32 one.
 *
 * @pre value > 0.
 *
 * @param ieee32            The given IEEE-754 binary32 representation.
 *
 * @returns Teju Jagua's binary representation value.
 */
teju32_fields_t
teju_ieee32_to_binary(teju32_fields_t ieee32);

/**
 * @brief Gets Teju Jagua's decimal representation of a float.
 *
 * @pre value > 0.
 *
 * @param value             The given float.
 *
 * @returns Teju Jagua's decimal representation of value.
 */
teju32_fields_t
teju_float_to_decimal(float value);

#ifdef __cplusplus
}
#endif

#endif // TEJU_TEJU_FLOAT_H_
