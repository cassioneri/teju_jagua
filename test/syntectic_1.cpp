#include "amaru/config.h"
#include "test/impl.hpp"

#define amaru_calculation_div10  amaru_syntectic_1
#define amaru_calculation_mshift amaru_syntectic_1
#define amaru_calculation_shift  32

#include "amaru/div10.h"
#include "amaru/mshift.h"

amaru_u1_t
amaru_multiply_1(amaru_u1_t const a, amaru_u1_t const b, amaru_u1_t* upper) {
  amaru_u2_t const p = amaru_u2_t(a) * amaru_u2_t(b);
  *upper = amaru_u1_t(p >> amaru_size);
  return amaru_u1_t(p);
}

using impl_t = amaru::test::syntectic_1_t;

int const impl_t::shift = amaru_calculation_shift;

amaru_u1_t
impl_t::div10(amaru_u1_t const m) {
  return amaru_div10(m);
}

amaru_u1_t
impl_t::mshift(amaru_u1_t const m, amaru_u1_t const u, amaru_u1_t const l) {
  return amaru_mshift(m, u, l);
}
