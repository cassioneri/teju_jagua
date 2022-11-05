#ifndef AMARU_AMAHU_INFIMUM_H_
#define AMARU_AMAHU_INFIMUM_H_

#ifdef __cplusplus
extern "C" {
#endif

static inline
amaru_limb1_t
infimum(amaru_limb1_t const m, amaru_limb1_t const upper,
  amaru_limb1_t const lower) {

  #if amaru_calculation_infimum > amaru_multiply_type
    #error "Value of 'amaru_calculation_infimum' is not supported."
  #endif

  #if amaru_calculation_infimum == amaru_built_in_4
    amaru_limb2_t const n = (((amaru_limb2_t) upper) << amaru_size) | lower;
    return (((amaru_limb4_t) n) * m) >> amaru_calculation_shift;

  #elif amaru_calculation_infimum == amaru_syntectic_2

    amaru_limb2_t const n = (((amaru_limb2_t) upper) << amaru_size) | lower;
    amaru_limb2_t u_prod;
    amaru_limb2_t const l_prod = amaru_multiply(n, m, &u_prod);

    if (amaru_calculation_shift >= 2 * amaru_size)
      return u_prod >> (amaru_calculation_shift - 2 * amaru_size);

    return u_prod << (2 * amaru_size - amaru_calculation_shift) |
      l_prod >> (amaru_calculation_shift - amaru_size);

  #elif amaru_calculation_infimum == amaru_built_in_2

    amaru_limb2_t const u_prod = ((amaru_limb2_t) upper) * m;
    amaru_limb2_t const l_prod = ((amaru_limb2_t) lower) * m;
    return (u_prod + (l_prod >> amaru_size)) >>
      (amaru_calculation_shift - amaru_size);

  #elif amaru_calculation_infimum == amaru_syntectic_1
    amaru_limb1_t ul_prod;
    (void) amaru_multiply(lower, m, &ul_prod);

    amaru_limb1_t uu_prod;
    amaru_limb1_t const lu_prod = amaru_multiply(upper, m, &uu_prod)
    amaru_limb1_t const l_prod  = ul_prod + lu_prod;

    amaru_limb1_t const carry  = l_prod < ul_prod;
    amaru_limb1_t const u_prod = uu_prod + carry;

    if (amaru_calculation_shift >= amaru_size)
      return u_prod >> (amaru_calculation_shift - 2 * amaru_size);

    return u_prod << (2 * amaru_size - amaru_calculation_shift) |
      (l_prod >> amaru_calculation_shift);

  #elif amaru_calculation_infimum == amaru_built_in_1

  #endif
}

#ifdef __cplusplus
}
#endif

#endif // AMARU_AMAHU_INFIMUM_H_
