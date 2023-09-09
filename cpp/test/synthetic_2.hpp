#ifndef AMARU_TEST_SYNTHETIC_2_HPP_
#define AMARU_TEST_SYNTHETIC_2_HPP_

extern "C" {

static inline
amaru_u2_t
amaru_multiply(amaru_u2_t const a, amaru_u2_t const b, amaru_u2_t* upper) {
  amaru_u4_t const p = amaru_u4_t(a) * amaru_u4_t(b);
  *upper = amaru_u2_t(p >> (2 * amaru_size));
  return amaru_u2_t(p);
}

}

#endif // AMARU_TEST_SYNTHETIC_2_HPP_
