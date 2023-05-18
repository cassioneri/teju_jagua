#ifndef AMARU_AMARU_DIV10_H_
#define AMARU_AMARU_DIV10_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Gets the quotient of the division by 10.
 *
 * @param m The dividend.
 * @returns m / 10.
 */
static inline
amaru_u1_t
div10(amaru_u1_t const m) {

  #if amaru_calculation_div10 == amaru_built_in_2

    #if amaru_multiply_type < amaru_built_in_2
      #error "Value of 'amaru_calculation_div10' isn't supported."
    #endif

    amaru_u1_t const inv10 = ((amaru_u1_t) - 1) / 10 + 1;
    return (((amaru_u2_t) inv10) * m) >> amaru_size;

  #elif amaru_calculation_div10 == amaru_syntectic_1

    #if amaru_multiply_type != amaru_syntectic_1
      #error "Value of 'amaru_calculation_div10' isn't supported."
    #endif

    amaru_u1_t const inv10 = ((amaru_u1_t) - 1) / 10 + 1;
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

  #else
    #error "Value of 'amaru_calculation_div10' isn't supported."
  #endif
}

#ifdef __cplusplus
}
#endif

#endif // AMARU_AMARU_DIV10_H_
