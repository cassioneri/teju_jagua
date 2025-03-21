// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

#include "cpp/test/impl.hpp"

#include <gtest/gtest.h>

namespace {

using namespace teju::test;

using impl_list_t = ::testing::Types<
  built_in_1_t, synthetic_1_t, built_in_2_t
>;

template <typename T>
class div10 : public testing::Test {
};

TYPED_TEST_SUITE(div10, impl_list_t);

// References:
// [1] Neri C, and Schneider L (2022), Euclidean Affine Functions and
// their application to calendar algorithms, Softw: Pract Exper., 2022
TYPED_TEST(div10, test_all_values) {

  using impl_t = TypeParam;

  // Applying theorem 4 of [1] for d = 10 and k = 16 gives:
  auto constexpr d  = 10;
  auto constexpr k  = 16;
  auto constexpr p2 = 1u << k;
  auto constexpr a  = p2 / 10 + 1;
  auto constexpr e  = d - p2 % d;
  static_assert(e < a, "Theorem 4 does not apply.");

  teju_u1_t constexpr U  = ((a + e - 1) / e) * d - 1;
  static_assert(U == 16389, "Invalid calculation of U of Theorem 4.");

  for (teju_u1_t m = 0; m < U; ++m)
    ASSERT_EQ(impl_t::div10(m), m / 10) << "Failed for m = " << m;
}

} // namespace <anonymous>
