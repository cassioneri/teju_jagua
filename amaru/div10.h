#ifndef AMARU_AMAHU_DIV10_H_
#define AMARU_AMAHU_DIV10_H_

#ifdef __cplusplus
extern "C" {
#endif

static inline
amaru_limb1_t
div10(amaru_limb1_t const m) {
  #if amaru_multiply_type >= amaru_built_in_2
    amaru_limb2_t const inv10 = ((amaru_limb1_t) -1) / 10 + 1;
    return (inv10 * m) >> amaru_size;
  #else
    return m / 10;
  #endif
}

#ifdef __cplusplus
}
#endif

#endif // AMARU_AMAHU_DIV10_H_
