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
 * \brief Returns the n_bits least significant bits of n.
 */
#define AMARU_LOWER_BITS(n, n_bits) \
  ((~(n^n) >> (CHAR_BIT * sizeof(n) - (n_bits))) & (n))

/**
 * \brief Returns the integer part of log_10(2^n).
 *
 * \pre n in [-70776, 70777[.
 */
static inline
int32_t log10_pow2(int32_t n) {
  int64_t const log10_2 = 1292913986;
  return n >= 0 ? log10_2 * n >> 32 : -log10_pow2(-n) - 1;
}

#ifdef __cplusplus
}
#endif
