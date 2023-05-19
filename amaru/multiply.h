#ifndef AMARU_AMARU_MULTIPLY_H_
#define AMARU_AMARU_MULTIPLY_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief
 *
 * @param
 * @returns
 */
#if (defined(amaru32_multiply_type) &&               \
     amaru32_multiply_type == amaru_syntectic_2 ) || \
    (defined(amaru64_multiply_type) &&               \
      amaru64_multiply_type == amaru_syntectic_1)

  static inline
  uint64_t
  amaru_multiply(uint64_t const a, uint64_t const b, uint64_t* upper) {
      return _umul128(a, b, upper);
  }

#endif

#ifdef __cplusplus
}
#endif

#endif // AMARU_AMARU_MULTIPLY_H_
