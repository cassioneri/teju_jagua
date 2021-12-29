#pragma once

#include <limits.h>
#include <stdint.h>
#include <assert.h>

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
 * \pre n in [-70776, 70777[.
 */
static inline
int32_t log10_pow2(int32_t n) {
  uint64_t const log10_2 = 1292913986;
  return log10_2 * n >> 32;
}

#ifdef __cplusplus
}
#endif
