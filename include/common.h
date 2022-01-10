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
 * \brief Returns the integer part of log_10(2^n).
 *
 * \pre -70776 <= n && n < 70777.
 */
#define AMARU_LOG10_POW2(n) \
  ((int32_t) ((((uint64_t) 1292913986) * ((int32_t) n)) >> 32))

/**
 * \brief Returns the integer part of log_2(10^n).
 *
 * \pre -55266 <= n && n < 55267.
 */
#define AMARU_LOG2_POW10(n) \
  ((int32_t) ((((uint64_t) 3566893131) * ((int32_t) n)) >> 30))

/**
 * \brief Returns the integer part of log_5(2^n).
 *
 * \pre -227267 <= n && n < 227268.
 */
#define AMARU_LOG5_POW2(n) \
  ((int32_t) ((((uint64_t) 1849741732) * ((int32_t) n)) >> 32))

#ifdef __cplusplus
}
#endif
