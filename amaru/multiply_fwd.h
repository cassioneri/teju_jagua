#ifndef AMARU_AMARU_MULTIPLY_FWD_H_
#define AMARU_AMARU_MULTIPLY_FWD_H_

/**
 * @file amaru/multiply_fwd.h
 *
 * Forward declaration of \c amaru_multiply.
 */

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

#ifdef __cplusplus
}
#endif

#endif // AMARU_AMARU_MULTIPLY_FWD_H_

