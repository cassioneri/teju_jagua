#include "amaru/common.h"

#include <boost/multiprecision/cpp_int.hpp>

#include <gtest/gtest.h>

#include <cstdint>

namespace {

using mp_int_t = boost::multiprecision::cpp_int;

TEST(log, log10_pow2_forward) {

  // e in [0, max[
  auto constexpr max = std::int32_t{112816};

  // Loop invariant: 10^f <= 2^e < 10^(f + 1)

  // e == 0:
  auto f     = std::int32_t{0};
  auto pow10 = mp_int_t{10}; // 10^(f + 1)
  auto pow2  = mp_int_t{1};  // 2^e

  // TIP: Not stopping at e = max is useful to discover what should be the
  // value of max.
  for (std::int32_t e = 0; e < max; ++e) {

    // Test the real code.
    ASSERT_EQ(log10_pow2(e), f) << "Note e = " << e;

    // Restore loop invariant for next iteration.
    pow2 *= 2;
    while (pow10 <= pow2) {
      pow10 *= 10;
      ++f;
    }
  }

  auto const e = max;
  EXPECT_NE(log10_pow2(e), f) << "Maximum " << max << " isn't sharp.";
}

TEST(log, log10_pow2_backward) {

  // e in [min, 0]
  auto constexpr min = std::int32_t{-112815};

  // Loop invariant: 10^f    <= 2^e    < 10^(f + 1)
  //                 10^(-f) >= 2^(-e) > 10^(-f - 1)

  // e == 0:
  auto f     = std::int32_t{0};
  auto pow10 = mp_int_t{1}; // 10^(-f)
  auto pow2  = mp_int_t{1}; // 2^(-e)

  // TIP: Not stopping at e = min is useful to discover what should be the
  // value of min.
  for (std::int32_t e = 0; e >= min; --e) {

    ASSERT_EQ(log10_pow2(e), f) << "Note e = " << e;

    // Restore loop invariant for next iteration.
    pow2 *= 2;
    while (pow10 < pow2) {
      pow10 *= 10;
      --f;
    }
  }

  auto const e = min - 1;
  EXPECT_NE(log10_pow2(e), f) << "Minimum " << min << " isn't sharp.";
}

TEST(log, log10_pow2_remainder) {

  for (std::int32_t e = -112815; e < 112816; ++e) {

    auto const f  = log10_pow2(e);
    auto const r  = log10_pow2_remainder(e);

    // r = e - e0, where e0 is the smallest value such that
    // log10_pow2(e0) = f.
    auto const e0 = e - static_cast<std::int32_t>(r);
    auto const f0 = log10_pow2(e0);
    auto const f1 = log10_pow2(e0 - 1);

    ASSERT_EQ(f0, f) << "Note: e = " << e << ", e0 = " << e0;
    ASSERT_LT(f1, f) << "Note: e = " << e << ", e0 = " << e0;
  }
}

TEST(log, log10_075_pow2_forward) {

  // e in [0, max[
  auto constexpr max = std::int32_t{111481};

  // Loop invariant:     10^f <= 3/4 * 2^e <     10^(f + 1)
  //                 4 * 10^f <=   3 * 2^e < 4 * 10^(f + 1)

  // e == 1, f == 0:
  auto f     = std::int32_t{0};
  auto pow10 = 4 * mp_int_t{10}; // 4 * 10^(f + 1)
  auto pow2  = 3 * mp_int_t{2};  // 3 *  2^e

  // TIP: Not stopping at e = max is useful to discover what should be the
  // value of max.
  for (std::int32_t e = 1; e < max; ++e) {

    // Test the real code.
    ASSERT_EQ(log10_075_pow2(e), f) << "Note e = " << e;

    // Restore loop invariant for next iteration.
    pow2 *= 2;
    while (pow10 <= pow2) {
      pow10 *= 10;
      ++f;
    }
  }

  auto const e = max;
  EXPECT_NE(log10_075_pow2(e), f) << "Maximum " << max << " isn't sharp.";
}

TEST(log, log10_075_pow2_backward) {

  // e in [min, 0]
  auto constexpr min = std::int32_t{-100849};

  // Loop invariant:     10^f    <= 3/4 * 2^e    <     10^(f + 1)
  //                     10^(-f) >= 4/3 * 2^(-e) >     10^(-f - 1)
  //                 3 * 10^(-f) >=   4 * 2^(-e) > 3 * 10^(-f - 1)

  // e == 0, f = -1:
  auto f     = std::int32_t{-1};
  auto pow10 = 3 * mp_int_t{10}; // 3 * 10^(-f)
  auto pow2  = 4 * mp_int_t{1};  // 4 *  2^(-e)

  // TIP: Not stopping at e = min is useful to discover what should be the
  // value of min.
  for (std::int32_t e = 0; e >= min; --e) {

    ASSERT_EQ(log10_075_pow2(e), f) << "Note e = " << e;

    // Restore loop invariant for next iteration.
    pow2 *= 2;
    while (pow10 < pow2) {
      pow10 *= 10;
      --f;
    }
  }

  auto const e = min - 1;
  EXPECT_NE(log10_075_pow2(e), f) << "Minimum " << min << " isn't sharp.";
}

TEST(log, log10_075_pow2_remainder) {

  for (std::int32_t e = -100849; e < 111481; ++e) {

    auto const f  = log10_075_pow2(e);
    auto const r  = log10_075_pow2_remainder(e);

    // r = e - e0, where e0 is the smallest value such that
    // log10_075_pow2(e0) = f.
    auto const e0 = e - static_cast<std::int32_t>(r);
    auto const f0 = log10_075_pow2(e0);
    auto const f1 = log10_075_pow2(e0 - 1);

    ASSERT_EQ(f0, f) << "Note: e = " << e << ", e0 = " << e0;
    ASSERT_LT(f1, f) << "Note: e = " << e << ", e0 = " << e0;
  }
}

} // namespace <anonymous>
