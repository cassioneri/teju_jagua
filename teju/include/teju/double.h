// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

/**
 * @file teju/src/double.h
 *
 * Tejú Jaguá and helpers for double values.
 */

#ifndef TEJU_TEJU_SRC_DOUBLE_H_
#define TEJU_TEJU_SRC_DOUBLE_H_

#include "../../src/config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Gets the binary representation of a given value.
 *
 * @param  value             The given value.
 *
 * @pre isfinite(value) && value > 0.
 *
 * @returns The binary representation of the given value.
 */
teju64_fields_t
teju_double_to_binary(double value);

/**
 * @brief Gets the decimal representation of a given value.
 *
 * @param  value            The given value.
 *
 * @pre isfinite(value) && value > 0.
 *
 * @returns The decimal representation of the given value.
 */
teju64_fields_t
teju_double_to_decimal(double value);

#ifdef __cplusplus
}
#endif

#endif // TEJU_TEJU_SRC_DOUBLE_H_
