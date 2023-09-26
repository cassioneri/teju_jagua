#ifndef AMARU_AMARU_COMMON_H_
#define AMARU_AMARU_COMMON_H_

/**
 * @file amaru/common.h
 *
 * Common functionalities used by Amaru and elsewhere (e.g., tests).
 */

#include <assert.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Gets the \e k least significant bits of \e n (i.e., \f$e\%2^k\f$.)
 *
 * @pre <tt>k < sizeof(n) * CHAR_BIT</tt>.
 *
 * @param n                 The value of \e n.
 * @param k                 The value of \e k.
 *
 * @returns The number \e k least significant bits of \e n.
 */
#define amaru_lsb(n, k) ((n) & (((0 * (n) + 1) << (k)) - 1))

/**
 * @brief Returns \f$2^n\f$ as a given type.
 *
 * @pre <tt>n < sizeof(type) * CHAR_BIT</tt>.
 *
 * @param type              The type.
 * @param n                 The value of \f$n\f$.
 *
 * @returns \f$2^n\f$.
 */
#define amaru_pow2(type, n) (((type) 1) << (n))

// Argument bounds of amaru_log10_pow2.
#define amaru_log10_pow2_min -112815
#define amaru_log10_pow2_max  112815

/**
 * @brief Returns the largest number \f$f\f$ such that \f$10^f <= 2^e\f$.
 *
 * @pre <tt>amaru_log10_pow2_min <= e && e <= amaru_log10_pow2_max</tt>.
 *
 * @param e                 The value of \f$e\f$.
 *
 * @returns The value of \f$f\f$ defined above.
 */
static inline
int32_t
amaru_log10_pow2(int32_t const e) {
  assert(amaru_log10_pow2_min <= e &&
    e <= amaru_log10_pow2_max &&
    "amaru_log10_pow2: invalid argument.");
  return (((uint64_t) 1292913987) * e) >> 32;
}

// Argument bounds of amaru_log10_pow2_residual.
#define amaru_log10_pow2_residual_min -112815
#define amaru_log10_pow2_residual_max  112815

/**
 * @brief Returns the residual \f$r = e - e_0\f$, where \f$e_0\f$ is the
 * smallest number such that \f$\lfloor\log_{10}(2^{e_0})\rfloor =
 * \lfloor\log_{10}(2^e)\rfloor\f$.
 *
 * @pre <tt>amaru_log10_pow2_residual_min <= e &&
 *      e <= amaru_log10_pow2_residual_max</tt>.
 *
 * @param e                 The value of \f$e\f$.
 *
 * @returns The residual \f$r\f$.
 */
static inline
uint32_t
amaru_log10_pow2_residual(int32_t const e) {
  assert(amaru_log10_pow2_residual_min <= e &&
    e <= amaru_log10_pow2_residual_max &&
    "amaru_log10_pow2_residual: invalid argument.");
  return ((uint32_t) (((uint64_t) 1292913987) * e)) / 1292913987;
}

// Argument bounds of amaru_log10_075_pow2.
#define amaru_log10_075_pow2_min -100849
#define amaru_log10_075_pow2_max  111480

/**
 * @brief Returns the largest number \f$f\f$ such that \f$10^f <= \frac{3}{4}
 * \cdot 2^e\f$.
 *
 * @pre <tt>amaru_log10_075_pow2_min <= e && e <= amaru_log10_075_pow2_max</tt>.
 *
 * @param e                 The value of \f$e\f$.
 *
 * @returns The value of \f$f\f$ defined above.
 */
static inline
int32_t
amaru_log10_075_pow2(int32_t const e) {
  assert(amaru_log10_075_pow2_min <= e &&
    e <= amaru_log10_075_pow2_max &&
    "amaru_log10_075_pow2: invalid argument.");
  return (((uint64_t) 1292913986) * e - 536607788) >> 32;
}

// Argument bounds of amaru_log10_075_pow2_residual.
#define amaru_log10_075_pow2_residual_min -112815
#define amaru_log10_075_pow2_residual_max  112815

/**
 * @brief Returns the residual \f$r = e - e_0\f$, where \f$e_0\f$ is the
 * smallest number such that \f$\lfloor\log_(\frac{3}{4}\cdot 2^{e_0})\rfloor =
 * \lfloor\log_{10}(2^e)\rfloor\f$.
 *
 * @pre <tt>amaru_log10_075_pow2_residual_min <= e &&
 *      e <= amaru_log10_075_pow2_residual_max</tt>.
 *
 * @param e                 The value of \f$e\f$.
 *
 * @returns The residual \f$r\f$.
 */
static inline
uint32_t
amaru_log10_075_pow2_residual(int32_t const e) {
  assert(amaru_log10_075_pow2_residual_min <= e &&
    e <= amaru_log10_075_pow2_residual_max &&
    "amaru_log10_075_pow2_residual: invalid argument.");
  return ((uint32_t) (((uint64_t) 1292913986) * e - 536607788)) / 1292913986;
}

#ifdef __cplusplus
}
#endif

#endif // AMARU_AMARU_COMMON_H_
