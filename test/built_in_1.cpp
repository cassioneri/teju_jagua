#define amaru_calculation_div10  amaru_built_in_1
#define amaru_calculation_mshift amaru_built_in_1
#define amaru_calculation_shift  32

#include "test/impl.hpp"

#include "amaru/div10.h"
#include "amaru/mshift.h"

using impl_t = amaru::test::built_in_1_t;

int const impl_t::shift = amaru_calculation_shift;

amaru_u1_t
impl_t::div10(amaru_u1_t const m) {
  return amaru_div10(m);
}

amaru_u1_t
impl_t::mshift(amaru_u1_t const m, amaru_u1_t const u, amaru_u1_t const l) {
  return amaru_mshift(m, u, l);
}
