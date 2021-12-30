#pragma once

#include <limits.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Returns 2^n as a given type.
 */
#define AMARU_POW2(type, n) (((type) 1) << (n))

/**
 * \brief Returns the k least significant bits (LSB) of n.
 */
#define AMARU_LSB(n, k) \
  ((~(n^n) >> (CHAR_BIT * sizeof(n) - (k))) & (n))

/**
 * \brief Returns the integer part of log_10(2^n).
 *
 * \pre -70776 <= n && n < 70777.
 */
static inline
int32_t log10_pow2(int32_t n) {
  uint64_t const log10_2_times_2_to_32 = 1292913986;
  return log10_2_times_2_to_32 * n >> 32;
}

/**
 * \brief Returns the integer part of log_2(10^n).
 *
 * \pre -55266 <= n && n < 55267.
 */
static inline
int32_t log2_pow10(int32_t n) {
  uint64_t const log2_10_times_2_to_30 = 3566893131;
  return log2_10_times_2_to_30 * n >> 30;
}

#ifdef __cplusplus
}
#endif
