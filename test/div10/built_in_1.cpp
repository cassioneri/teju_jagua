#include "amaru/config.h"
#include "test/div10/div10.hpp"

#define amaru_calculation_div10 amaru_built_in_1
#define amaru_multiply_type     amaru_built_in_1
#include "amaru/div10.h"

template <>
amaru_u1_t
amaru::test::div10<amaru_built_in_1>(amaru_u1_t const m) {
  return amaru_div10(m);
}
