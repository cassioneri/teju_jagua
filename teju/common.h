// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2024 Cassio Neri <cassio.neri@gmail.com>

/**
 * @file teju/common.h
 *
 * Common functionalities used by Teju Jagua and elsewhere (e.g., tests).
 */

#ifndef TEJU_TEJU_COMMON_H_
#define TEJU_TEJU_COMMON_H_

#include <assert.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Gets the k least significant bits of n (i.e., e % 2^k.)
 *
 * @pre k < sizeof(n) * CHAR_BIT.
 *
 * @param n                 The value of n.
 * @param k                 The value of k.
 *
 * @returns The k least significant bits of n.
 */
#define teju_lsb(n, k) ((n) & (((0 * (n) + 1) << (k)) - 1))

/**
 * @brief Returns 2^e as a given type.
 *
 * @pre e < sizeof(type) * CHAR_BIT.
 *
 * @param type              The type.
 * @param n                 The exponent e.
 *
 * @returns 2^e.
 */
#define teju_pow2(type, e) (((type) 1) << (e))

// Argument bounds of teju_log10_pow2.
#define teju_log10_pow2_min -112815
#define teju_log10_pow2_max  112815

/**
 * @brief Returns the largest exponent f such that 10^f <= 2^e.
 *
 * @pre teju_log10_pow2_min <= e && e <= teju_log10_pow2_max.
 *
 * @param e                 The exponent e.
 *
 * @returns The exponent f.
 */
static inline
int32_t
teju_log10_pow2(int32_t const e) {
  return (((uint64_t) 1292913987) * e) >> 32;
}

/**
 * @brief Returns the residual r = e - e_0, where e_0 is the smallest exponent
 * such that the integer part of log_10(2^e_0) matches that of log_10(2^e).
 *
 * @pre teju_log10_pow2_min <= e && e <= teju_log10_pow2_max.
 *
 * @param e                 The exponent e.
 *
 * @returns The residual r.
 */
static inline
uint32_t
teju_log10_pow2_residual(int32_t const e) {
  return ((uint32_t) (((uint64_t) 1292913987) * e)) / 1292913987;
}

// Argument bounds of teju_log10_075_pow2.
#define teju_log10_075_pow2_min -100849
#define teju_log10_075_pow2_max  111480

#ifdef __cplusplus
}
#endif

#endif // TEJU_TEJU_COMMON_H_