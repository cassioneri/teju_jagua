#ifndef AMARU_AMARU_DIV10_H_
#define AMARU_AMARU_DIV10_H_

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
amaru_u1_t
div10(amaru_u1_t const m) {

  #if amaru_calculation_div10 == amaru_built_in_2

    amaru_u1_t const inv10 = ((amaru_u1_t) -1) / 10 + 1;
    return (((amaru_u2_t) inv10) * m) >> amaru_size;

  #elif amaru_calculation_div10 == amaru_syntectic_1

    amaru_u1_t const inv10 = ((amaru_u1_t) -1) / 10 + 1;
    amaru_u1_t upper;
    (void) amaru_multiply(inv10, m, &upper);
    return upper;

  #elif amaru_calculation_div10 == amaru_built_in_1

    amaru_u1_t const p2   = ((amaru_u1_t) 1) << (amaru_size / 2);
    amaru_u1_t const inv5 = (p2 - 1) / 5;
    amaru_u1_t const u    = m / p2;
    amaru_u1_t const l    = m % p2;

    return (((l * (inv5 + 1)) / p2 + l * inv5 + u * (inv5 + 1)) / p2 +
      u * inv5) / 2;

  #endif
}

#ifdef __cplusplus
}
#endif

#endif // AMARU_AMARU_DIV10_H_
