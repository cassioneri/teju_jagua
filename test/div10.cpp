#include "amaru/types.h"

#include <cstdint>

// For amaru_calculation_div10 == amaru_built_in_2
#define amaru_calculation_div10 amaru_built_in_2
#define amaru_multiply_type     amaru_built_in_2
#define amaru_size              32
#define amaru_limb1_t           std::uint32_t
#define amaru_limb2_t           std::uint64_t
#define div10                   div10_built_in_2
#include "amaru/div10.h"
#undef AMARU_AMAHU_DIV10_H_
#undef amaru_calculation_div10
#undef amaru_multiply_type
#undef amaru_size
#undef amaru_limb1_t
#undef amaru_limb2_t
#undef div10

// For amaru_calculation_div10 == amaru_syntectic_1
#define amaru_calculation_div10 amaru_syntectic_1
#define amaru_multiply_type     amaru_syntectic_1
#define amaru_size              64
#define amaru_limb1_t           std::uint64_t
#define div10                   div10_syntectic_1
#include "amaru/div10.h"
#undef AMARU_AMAHU_DIV10_H_
#undef amaru_calculation_div10
#undef amaru_multiply_type
#undef amaru_size
#undef amaru_limb1_t
#undef div10

// For amaru_calculation_div10 == amaru_built_in_1
#define amaru_calculation_div10 amaru_built_in_1
#define amaru_multiply_type     amaru_built_in_1
#define amaru_size              32
#define amaru_limb1_t           std::uint32_t
#define div10                   div10_built_in_1
#include "amaru/div10.h"
#undef AMARU_AMAHU_DIV10_H_
#undef amaru_calculation_div10
#undef amaru_multiply_type
#undef amaru_size
#undef amaru_limb1_t
#undef div10

#include <gtest/gtest.h>

#include <cstdint>

namespace {

// References:
// [1] Neri C, and Schneider L (2022), Euclidean Affine Functions and
// Applications to Calendar Algorithms, Softw: Pract Exper., 2022

TEST(div10, built_in_2) {

  // Theorem 4 of [1] for d = 10 and k = 32 gives this maximum value:
  auto constexpr max = std::uint32_t{1073741829};

  for (std::uint32_t m = 0; m < max; ++ m)
    ASSERT_EQ(div10_built_in_2(m), m / 10) << "Failed for m = " << m;
}

TEST(div10, syntectic_1) {

  for (std::uint64_t m = 0; m < 1000000; ++m)
    ASSERT_EQ(div10_syntectic_1(m), m / 10) << "Failed for m = " << m;

  // Theorem 4 of [1] for d = 10 and k = 64 gives this maximum value:
  auto constexpr max = std::uint64_t{4611686018427387909};

  for (std::uint64_t m = max - 1000000; m < max; ++m)
    ASSERT_EQ(div10_syntectic_1(m), m / 10) << "Failed for m = " << m;
}

TEST(div10, built_in_1) {

  auto constexpr max = std::uint32_t{1} << 28;
  for (std::uint32_t m = 0; m < max; ++m)
    ASSERT_EQ(div10_built_in_1(m), m / 10) << "Failed for m = " << m;
}

} // namespace <anonymous>