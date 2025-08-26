// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

#include "../../teju/src/common.h"

#include <boost/multiprecision/cpp_int.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <cstdint>
#include <limits>

namespace {

using mp_int_t = boost::multiprecision::cpp_int;

TEST(log, teju_log10_pow2_forward) {

  // e in [0, max]
  auto constexpr max = std::int32_t{teju_log10_pow2_max};

  // Loop invariant: 10^f <= 2^e < 10^(f + 1)

  // e == 0:
  auto f     = std::int32_t{0};
  auto pow10 = mp_int_t{10}; // 10^(f + 1)
  auto pow2  = mp_int_t{1};  // 2^e

  // Sanity check for the test itself.
  ASSERT_LT(max, std::numeric_limits<std::int32_t>::max());

  // TIP: Not stopping at e = max is useful to discover what should be the value
  // of max.
  for (std::int32_t e = 0; e <= max; ++e) {

    // Test the real code.
    ASSERT_EQ(teju_log10_pow2(e), f) << "Note e = " << e;

    // Restore loop invariant for next iteration.
    pow2 *= 2;
    while (pow10 <= pow2) {
      pow10 *= 10;
      ++f;
    }
  }

  auto constexpr e = max + 1;
  EXPECT_NE(teju_log10_pow2(e), f) << "Maximum " << max << " isn't sharp.";
}

TEST(log, teju_log10_pow2_backward) {

  // e in [min, 0]
  auto constexpr min = std::int32_t{teju_log10_pow2_min};

  // Loop invariant: 10^f    <= 2^e    < 10^(f + 1)
  //                 10^(-f) >= 2^(-e) > 10^(-f - 1)

  // e == 0:
  auto f     = std::int32_t{0};
  auto pow10 = mp_int_t{1}; // 10^(-f)
  auto pow2  = mp_int_t{1}; // 2^(-e)

  // Sanity check for the test itself.
  ASSERT_GT(min, std::numeric_limits<std::int32_t>::min());

  // TIP: Not stopping at e = min is useful to discover what should be the value
  // of min.
  for (std::int32_t e = 0; e >= min; --e) {

    ASSERT_EQ(teju_log10_pow2(e), f) << "Note e = " << e;

    // Restore loop invariant for next iteration.
    pow2 *= 2;
    while (pow10 < pow2) {
      pow10 *= 10;
      --f;
    }
  }

  auto constexpr e = min - 1;
  EXPECT_NE(teju_log10_pow2(e), f) << "Minimum " << min << " isn't sharp.";
}

TEST(log, teju_log10_pow2_residual) {

  auto constexpr min = std::int32_t{teju_log10_pow2_min};
  auto constexpr max = std::int32_t{teju_log10_pow2_max};

  // Sanity check for the test itself.
  ASSERT_LT(max, std::numeric_limits<std::int32_t>::max());

  for (std::int32_t e = min; e <= max; ++e) {

    auto const f  = teju_log10_pow2(e);
    auto const r  = teju_log10_pow2_residual(e);

    // r = e - e0, where e0 is the smallest value such that
    // teju_log10_pow2(e0) = f.
    auto const e0 = e - static_cast<std::int32_t>(r);
    auto const f0 = teju_log10_pow2(e0);
    auto const f1 = teju_log10_pow2(e0 - 1);

    ASSERT_EQ(f0, f) << "Note: e = " << e << ", e0 = " << e0;
    ASSERT_LT(f1, f) << "Note: e = " << e << ", e0 = " << e0;
  }
}

} // namespace <anonymous>
