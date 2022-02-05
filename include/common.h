#pragma once

#include <limits.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Returns the k least significant bits of n.
 */
// ~(n^n) = constant of the same type of n with all bits set.
#define AMARU_LSB(n, k) \
  ((~(n^n) >> (CHAR_BIT * sizeof(n) - (k))) & (n))

/**
 * \brief Returns 2^n as a given type.
 */
#define AMARU_POW2(type, n) \
  (((type) 1) << (n))

/**
 * \brief Returns the largest number f such that 10^f <= 2^e.
 *
 * \pre -112815 <= e && e < 112816.
 */
static inline
int32_t log10_pow2(int32_t e) {
  return (int32_t) ((((uint64_t) 1292913987) * e) >> 32);
}

/**
 * \brief Returns the the remainder e - e0, where e0 is the smallest number such
 * that log10_pow2(e0) == log10_pow2(e).
 *
 * \pre -112815 <= e && e < 112816.
 */
static inline
uint32_t log10_pow2_remainder(int32_t e) {
  return ((uint32_t) (((uint64_t) 1292913987) * (e))) / 1292913987;
}

#ifdef __cplusplus
}
#endif
