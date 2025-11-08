// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

/**
 * @file teju/src/common.h
 *
 * Common functionalities used by Tejú Jaguá and elsewhere (e.g. tests).
 */

#ifndef TEJU_TEJU_SRC_COMMON_H_
#define TEJU_TEJU_SRC_COMMON_H_

#include <assert.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Gets the width (number of bits) of a given type.
 *
 * @tparam T                The given type.
 *
 * @returns The width.
 */
#define teju_width_of(T) (sizeof(T) * CHAR_BIT)

/**
 * @brief Gets 2^e as a given type.
 *
 * @note Here ^ denotes exponentiation (not bit-wise xor).
 *
 * @tparam UInt             The given type.
 * @param  e                The exponent e.
 *
 * @pre UInt is an unsigned integer type and e < teju_width_of(UInt).
 *
 * @returns 2^e.
 */
#define teju_pow2(UInt, e) ((UInt) ((UInt) 1u << (e)))

 /**
 * @brief Gets the k least-significant bits of n (i.e. n % 2^k.)
 *
 * @note Here ^ denotes exponentiation (not bit-wise xor).
 *
 * @tparam UInt             The type of n.
 * @param  n                The value of n.
 * @param  k                The value of k.
 *
 * @pre UInt is an unsigned integer type and k < teju_width_of(UInt).
 *
 * @returns The k least-significant bits of n.
 */
#define teju_lsb(UInt, n, k) ((n) % teju_pow2(UInt, k))

// Argument bounds of teju_log10_pow2.
#define teju_log10_pow2_min (-112815)
#define teju_log10_pow2_max   112815

/**
 * @brief Gets the largest integer f such that 10^f <= 2^e, i.e. the integer
 *        part of log_10(2^e).
 *
 * @note Here ^ denotes exponentiation (not bit-wise xor).
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
  assert(teju_log10_pow2_min <= e && e <= teju_log10_pow2_max);
  return (int32_t) ((int64_t) 1292913987u * e >> 32u);
}

/**
 * @brief Gets the residual r = e - e_0, where e_0 is the smallest integer such
 *        that teju_log10_pow2(e_0) == teju_log10_pow2(e).
 *
 * @param  e                The value of e.
 *
 * @pre teju_log10_pow2_min <= e && e <= teju_log10_pow2_max.
 *
 * @returns The residual r.
 */
static inline
uint32_t
teju_log10_pow2_residual(int32_t const e) {
  assert(teju_log10_pow2_min <= e && e <= teju_log10_pow2_max);
  return (uint32_t) ((int64_t) 1292913987u * e) / 1292913987u;
}

#ifdef __cplusplus
}
#endif

#endif // TEJU_TEJU_SRC_COMMON_H_
