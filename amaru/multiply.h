#ifndef AMARU_AMARU_MULTIPLY_H_
#define AMARU_AMARU_MULTIPLY_H_

/**
 * @file amaru/multiply.h
 *
 * Custom multiplication functionalities.
 */

#include "amaru/multiply_fwd.h"

#if defined(_MSC_VER)

#include <intrin.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Calculates the 128-bits product of two 64-bits unsigned numbers.
 *
 * @param a The 1st multiplicand.
 * @param b The 2nd multiplicand.
 * @param upper On exit the value of the highest 64-bits of the product.
 *
 * @returns The lower 64-bits value of the product.
 */
inline static
uint64_t
amaru_multiply(uint64_t const a, uint64_t const b, uint64_t* upper) {
  return _umul128(a, b, upper);
}

#ifdef __cplusplus
}
#endif

#endif // defined(_MSC_VER)
#endif // AMARU_AMARU_MULTIPLY_H_
