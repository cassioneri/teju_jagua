// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

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
 * @brief A C99 compliant static assert.
 *
 * @param  c                The condition to be checked.
 * @param  msg              The error message to be (hopefully) displayed.
 */
#define teju_static_assert(c, msg) \
do {                               \
  const int _ = (c) ? 1 : -1;      \
  int                              \
  static_assert_failed[_];         \
  (void) static_assert_failed;     \
} while(false)

/**
 * @brief Returns the k least significant bits of n (i.e. n % 2^k.)
 *
 * @param  n                The value of n.
 * @param  k                The value of k.
 *
 * @pre k < sizeof(n) * CHAR_BIT.
 *
 * @returns The k least significant bits of n.
 */
#define teju_lsb(n, k) ((n) & (((0u * (n) + 1u) << (k)) - 1u))

/**
 * @brief Returns 2^e as a given type.
 *
 * @param  type             The type.
 * @param  n                The exponent e.
 *
 * @pre e < sizeof(type) * CHAR_BIT.
 *
 * @returns 2^e.
 */
#define teju_pow2(type, e) ((type) ((type) 1u) << (e))

// Argument bounds of teju_log10_pow2.
#define teju_log10_pow2_min -112815
#define teju_log10_pow2_max  112815

/**
 * @brief Returns the largest exponent f such that 10^f <= 2^e.
 *
 * @param  e                 The exponent e.
 *
 * @pre teju_log10_pow2_min <= e && e <= teju_log10_pow2_max.
 *
 * @returns The exponent f.
 */
static inline
int32_t
teju_log10_pow2(int32_t const e) {
  return (((uint64_t) 1292913987u) * e) >> 32u;
}

/**
 * @brief Returns the residual r = e - e_0, where e_0 is the smallest exponent
 * such that the integer part of log_10(2^e_0) matches that of log_10(2^e).
 *
 * @param  e                The exponent e.
 *
 * @pre teju_log10_pow2_min <= e && e <= teju_log10_pow2_max.
 *
 * @returns The residual r.
 */
static inline
uint32_t
teju_log10_pow2_residual(int32_t const e) {
  return ((uint32_t) (((uint64_t) 1292913987u) * e)) / 1292913987u;
}

#ifdef __cplusplus
}
#endif

#endif // TEJU_TEJU_COMMON_H_
