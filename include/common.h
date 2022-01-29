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
 * \pre -70776 <= e && e < 70777.
 */
#define AMARU_LOG10_POW2(e) \
  ((int32_t) ((((uint64_t) 1292913986) * (e)) >> 32))

/**
 * \brief Returns the the remainder e - e0, where e0 is the smallest number such
 * that AMARU_LOG10_POW2(e0) == AMARU_LOG10_POW2(e).
 *
 * \pre -70776 <= e && e < 70777.
 */
#define AMARU_LOG10_POW2_REMAINDER(e) \
  (((uint32_t) (((uint64_t) 1292913986) * (e))) / 1292913986)

#ifdef __cplusplus
}
#endif
