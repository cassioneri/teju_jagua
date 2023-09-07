#include "amaru/config.h"
#include "test/impl.hpp"

#define amaru_calculation_mshift amaru_synthetic_2
#define amaru_calculation_shift  24

// Avoids ODR violation.
#define amaru_multiply amaru_multiply_test_synthetic_2

#include "amaru/mshift.h"

using impl_t = amaru::test::synthetic_2_small_shift_t;

int const impl_t::shift = amaru_calculation_shift;

amaru_u1_t
impl_t::mshift(amaru_u1_t const m, amaru_u1_t const u, amaru_u1_t const l) {
  return amaru_mshift(m, u, l);
}
