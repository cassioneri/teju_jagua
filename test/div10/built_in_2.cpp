#include "amaru/config.h"
#include "test/div10/div10.hpp"

#define amaru_calculation_div10 amaru_built_in_2
#define amaru_multiply_type     amaru_built_in_2
#include "amaru/div10.h"

template <>
amaru_u1_t
amaru::test::div10<amaru_built_in_2>(amaru_u1_t const m) {
  return amaru_div10(m);
}
