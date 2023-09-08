#ifndef AMARU_AMARU_MULTIPLY_H_
#define AMARU_AMARU_MULTIPLY_H_

/**
 * @file amaru/multiply.h
 *
 * Custom multiplication functionalities.
 */

#if defined(_MSC_VER)
  #include <intrin.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if amaru_multiply_type == amaru_synthetic_1

  static inline
  amaru_u1_t
  amaru_multiply(amaru_u1_t const a, amaru_u1_t const b, amaru_u1_t* upper);

#elif amaru_multiply_type == amaru_synthetic_2

  static inline
  amaru_u2_t
  amaru_multiply(amaru_u2_t const a, amaru_u2_t const b, amaru_u2_t* upper);

#endif

#if defined(_MSC_VER) && !defined(amaru_do_not_define_amaru_multiply)

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

#endif // defined(_MSC_VER)

#ifdef __cplusplus
}
#endif

#endif // AMARU_AMARU_MULTIPLY_H_
