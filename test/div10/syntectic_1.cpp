#include "amaru/config.h"
#include "test/div10/div10.hpp"

#define amaru_calculation_div10 amaru_syntectic_1
#define amaru_multiply_type     amaru_syntectic_1
#include "amaru/div10.h"

amaru_u1_t
amaru_multiply_1(amaru_u1_t const a, amaru_u1_t const b, amaru_u1_t* upper) {
  auto const p = amaru_u2_t(a) * amaru_u2_t(b);
  *upper = amaru_u1_t(p >> amaru_size);
  return amaru_u1_t(p);
}

template <>
amaru_u1_t
amaru::test::div10<amaru_syntectic_1>(amaru_u1_t const m) {
  return amaru_div10(m);
}
