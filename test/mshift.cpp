#include "test/impl.hpp"

#include <gtest/gtest.h>

namespace {

using namespace amaru::test;

using impl_list_t = ::testing::Types<
  built_in_1_t, built_in_1_small_shift_t, 
  synthetic_1_t, synthetic_1_small_shift_t,
  built_in_2_t,
  synthetic_2_t, synthetic_2_small_shift_t,
  built_in_4_t
>;

template <typename T>
class mshift : public testing::Test {
};

TYPED_TEST_SUITE(mshift, impl_list_t);

TYPED_TEST(mshift, test_one_billion_values) {

  using impl_t = TypeParam;

  for (amaru_u1_t m = 0; m < 1000; ++m) {
    for (amaru_u1_t u = 0; u < 1000; ++u) {
      for (amaru_u1_t l = 0; l < 1000; ++l) {

        amaru_u1_t const actual   = impl_t::mshift(m, u, l);
        amaru_u1_t const expected = (((amaru_u4_t(u) << amaru_size) + l) * m) >>
          impl_t::shift;

        ASSERT_EQ(actual, expected) << "Failed for "
          "m = " << m << ", u = " << u << ", l = " << l << '\n';
      }
    }
  }
}

} // namespace <anonymous>
