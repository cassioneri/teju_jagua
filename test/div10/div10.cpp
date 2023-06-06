#include "amaru/config.h"
#include "test/div10/div10.hpp"
#include "test/impl.hpp"

#include <gtest/gtest.h>

namespace {

using namespace amaru::test;

using impl_list_t = ::testing::Types<
    built_in_1
  , syntectic_1
  , built_in_2
>;

template <typename T>
class div10 : public testing::Test {
};

TYPED_TEST_SUITE(div10, impl_list_t);

// References:
// [1] Neri C, and Schneider L (2022), Euclidean Affine Functions and
// their application to calendar algorithms, Softw: Pract Exper., 2022
TYPED_TEST(div10, test_all_values) {

  auto constexpr id = TypeParam::value;

  // Theorem 4 of [1] for d = 10 and k = 32 gives this maximum value:
  static amaru_u1_t constexpr max = 1073741829;

  for (amaru_u1_t m = 0; m < max; ++m)
    ASSERT_EQ(amaru::test::div10<id>(m), m / 10) << "Failed for m = " << m;
}

} // namespace <anonymous>
