#include "cpp/test/impl.hpp"

// Other macros required by amaru/mshift.h.
#define amaru_calculation_mshift amaru_built_in_4
#define amaru_calculation_shift  32

#include "amaru/mshift.h"

using impl_t = amaru::test::built_in_4_t;

int const impl_t::shift = amaru_calculation_shift;

amaru_u1_t
impl_t::mshift(amaru_u1_t const m, amaru_u1_t const u, amaru_u1_t const l) {
  return amaru_mshift(m, u, l);
}
