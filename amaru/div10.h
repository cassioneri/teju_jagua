#ifndef AMARU_AMAHU_DIV10_H_
#define AMARU_AMAHU_DIV10_H_

#ifdef __cplusplus
extern "C" {
#endif

#if (amaru_calculation_div10 % 2 == 0 &&                         \
    amaru_calculation_div10 >  2 * (amaru_multiply_type / 2)     \
  ) || (                                                         \
    amaru_calculation_div10 % 2 == 1 &&                          \
    amaru_calculation_div10 != 2 * (amaru_multiply_type / 2) + 1 \
  )
  #error "Value of 'amaru_calculation_div10' isn't supported."
#endif

static inline
amaru_limb1_t
div10(amaru_limb1_t const m) {

  #if amaru_calculation_div10 == amaru_built_in_2

    amaru_limb1_t const inv10 = ((amaru_limb1_t) -1) / 10 + 1;
    return (((amaru_limb2_t) inv10) * m) >> amaru_size;

  #elif amaru_calculation_div10 == amaru_syntectic_1

    amaru_limb1_t const inv10 = ((amaru_limb1_t) -1) / 10 + 1;
    amaru_limb1_t upper;
    (void) amaru_multiply(inv10, m, &upper);
    return upper;

  #elif amaru_calculation_div10 == amaru_built_in_1

    return m / 10;

  #endif
}

#ifdef __cplusplus
}
#endif

#endif // AMARU_AMAHU_DIV10_H_
