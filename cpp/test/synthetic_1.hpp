#ifndef AMARU_TEST_SYNTHETIC_1_HPP_
#define AMARU_TEST_SYNTHETIC_1_HPP_

extern "C" {

static inline
amaru_u1_t
amaru_multiply(amaru_u1_t const a, amaru_u1_t const b, amaru_u1_t* upper) {
  amaru_u2_t const p = amaru_u2_t(a) * amaru_u2_t(b);
  *upper = amaru_u1_t(p >> amaru_size);
  return amaru_u1_t(p);
}

}

#endif // AMARU_TEST_SYNTHETIC_1_HPP_