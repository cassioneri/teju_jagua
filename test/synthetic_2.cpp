#include "amaru/config.h"
#include "test/impl.hpp"

#define amaru_calculation_mshift amaru_synthetic_2
#define amaru_calculation_shift  32

#include "amaru/mshift.h"

amaru_u2_t
amaru_multiply_2(amaru_u2_t const a, amaru_u2_t const b, amaru_u2_t* upper) {
  amaru_u4_t const p = amaru_u4_t(a) * amaru_u4_t(b);
  *upper = amaru_u2_t(p >> (2 * amaru_size));
  return amaru_u2_t(p);
}

using impl_t = amaru::test::synthetic_2_t;

int const impl_t::shift = amaru_calculation_shift;

amaru_u1_t
impl_t::mshift(amaru_u1_t const m, amaru_u1_t const u, amaru_u1_t const l) {
  return amaru_mshift(m, u, l);
}
