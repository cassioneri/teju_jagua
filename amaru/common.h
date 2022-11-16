#ifndef AMARU_AMARU_COMMON_H_
#define AMARU_AMARU_COMMON_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Returns the k least significant bits of n.
 */
#define amaru_lsb(n, k) ((n) & (((0 * (n) + 1) << (k)) - 1))

/**
 * \brief Returns 2^n as a given type.
 */
#define amaru_pow2(type, n) (((type) 1) << (n))

/**
 * \brief Returns the largest number f such that 10^f <= 2^e.
 *
 * \pre -112815 <= e && e < 112816.
 */
static inline
int32_t
log10_pow2(int32_t const e) {
  return (((uint64_t) 1292913987) * e) >> 32;
}

/**
 * \brief Returns the residual e - e0, where e0 is the smallest number such
 * that log10_pow2(e0) == log10_pow2(e).
 *
 * \pre -112815 <= e && e < 112816.
 */
static inline
uint32_t
log10_pow2_residual(int32_t const e) {
  return ((uint32_t) (((uint64_t) 1292913987) * e)) / 1292913987;
}

/**
 * \brief Returns the largest number f such that 10^f <= 0.75 * 2^e.
 *
 * \pre -100849 <= e && e < 111481.
 */
static inline
int32_t
log10_075_pow2(int32_t const e) {
  return (((uint64_t) 1292913986) * e - 536607788) >> 32;
}

/**
 * \brief Returns the residual r = e - e0, where e0 is the smallest number
 * such that log10_075_pow2(e0) == log10_pow2(e).
 *
 * \pre -112815 <= e && e < 112816.
 */
static inline
uint32_t
log10_075_pow2_residual(int32_t const e) {
  return ((uint32_t) (((uint64_t) 1292913986) * e - 536607788)) / 1292913986;
}

#ifdef __cplusplus
}
#endif

#endif // AMARU_AMARU_COMMON_H_
