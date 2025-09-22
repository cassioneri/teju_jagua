// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

#include "impl.hpp"

#include <gtest/gtest.h>

namespace {

using namespace teju::test;

template <typename TImpl>
void check(teju_u1_t const m, teju_u1_t const u, teju_u1_t const l) {

  teju_multiplier_t const M = { u, l };

  teju_u1_t const actual   = TImpl::mshift(m, M);
  teju_u1_t const expected = (((teju_u4_t(u) << teju_width) + l) * m) >>
    (2 * teju_width);

  EXPECT_EQ(actual, expected) << "Failed for "
    "m = " << m << ", u = " << u << ", l = " << l << '\n';
}

using impl_list_t = ::testing::Types<built_in_1_t, synthetic_1_t, built_in_2_t,
  synthetic_2_t, built_in_4_t>;

template <typename /*TUint*/>
class mshift : public testing::Test {
};

TYPED_TEST_SUITE(mshift, impl_list_t);

TYPED_TEST(mshift, test_one_billion_values) {

  for (teju_u1_t m = 0; m < 1000; ++m)
    for (teju_u1_t u = 0; u < 1000; ++u)
      for (teju_u1_t l = 0; l < 1000 && !this->HasFailure(); ++l)
        check<TypeParam>(m, u, l);
}

TYPED_TEST(mshift, ad_hoc) {
  teju_u1_t const m = 0;
  teju_u1_t const u = 0;
  teju_u1_t const l = 0;
  check<TypeParam>(m, u, l);
}

} // namespace <anonymous>
