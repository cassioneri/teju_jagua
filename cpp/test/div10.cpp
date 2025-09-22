// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

#include "impl.hpp"

#include <gtest/gtest.h>

namespace {

using namespace teju::test;

using impl_list_t = ::testing::Types<synthetic_1_t, built_in_2_t>;

template <typename /*TFloat*/>
class div10 : public testing::Test {
};

TYPED_TEST_SUITE(div10, impl_list_t);

// References:
// [1] Neri C, and Schneider L (2022), Euclidean Affine Functions and
// their application to calendar algorithms, Softw: Pract Exper., 2022
TYPED_TEST(div10, test_all_values) {

  using impl_t = TypeParam;

  // Applying theorem 4 of [1] for d = 10 and k = 16 gives:
  teju_u1_t constexpr d            = 10u;
  teju_u1_t constexpr p2_k_minus_1 = teju_u1_t(-1);
  teju_u1_t constexpr a            = p2_k_minus_1 / 10u + 1u;
  teju_u1_t constexpr epsilon      = d - (p2_k_minus_1 % d + 1u) % d;
  teju_u1_t constexpr U = 1u * ((a + epsilon - 1u) / epsilon) * d - 1u;

  static_assert(epsilon <= a, "Theorem 4 does not apply.");
  static_assert(U == 16389u, "Invalid calculation of U of Theorem 4.");

  for (teju_u1_t m = 0; m < U; ++m)
    ASSERT_EQ(impl_t::div10(m), m / 10) << "Failed for m = " << m;
}

} // namespace <anonymous>
