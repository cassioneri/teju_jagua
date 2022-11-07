#include "amaru/types.h"

#include <cstdint>

// For amaru_calculation_infimum == amaru_built_in_4
#define amaru_calculation_infimum amaru_built_in_4
#define amaru_multiply_type       amaru_built_in_4
#define amaru_size                16
#define amaru_limb1_t             std::uint16_t
#define amaru_limb2_t             std::uint32_t
#define amaru_limb4_t             std::uint64_t
#define amaru_calculation_shift   32
#define infimum                   infimum_built_in_4
#include "amaru/infimum.h"
#undef AMARU_AMAHU_INFIMUM_H_
#undef amaru_calculation_infimum
#undef amaru_multiply_type
#undef amaru_size
#undef amaru_limb1_t
#undef amaru_limb2_t
#undef amaru_limb4_t
#undef amaru_calculation_shift
#undef infimum

// For amaru_calculation_infimum == amaru_syntectic_2
#define amaru_calculation_infimum amaru_syntectic_2
#define amaru_multiply_type       amaru_syntectic_2
#define amaru_size                32
#define amaru_limb1_t             std::uint32_t
#define amaru_limb2_t             std::uint64_t
#define amaru_calculation_shift   64
#define infimum                   infimum_syntectic_2
#include "amaru/infimum.h"
#undef AMARU_AMAHU_INFIMUM_H_
#undef amaru_calculation_infimum
#undef amaru_multiply_type
#undef amaru_size
#undef amaru_limb1_t
#undef amaru_limb2_t
#undef amaru_calculation_shift
#undef infimum

// For amaru_calculation_infimum == amaru_built_in_2
#define amaru_calculation_infimum amaru_built_in_2
#define amaru_multiply_type       amaru_built_in_2
#define amaru_size                32
#define amaru_limb1_t             std::uint32_t
#define amaru_limb2_t             std::uint64_t
#define amaru_calculation_shift   64
#define infimum                   infimum_built_in_2
#include "amaru/infimum.h"
#undef AMARU_AMAHU_INFIMUM_H_
#undef amaru_calculation_infimum
#undef amaru_multiply_type
#undef amaru_size
#undef amaru_limb1_t
#undef amaru_limb2_t
#undef amaru_calculation_shift
#undef infimum

// For amaru_calculation_infimum == amaru_syntectic_1
#define amaru_calculation_infimum amaru_syntectic_1
#define amaru_multiply_type       amaru_syntectic_1
#define amaru_size                64
#define amaru_limb1_t             std::uint64_t
#define amaru_calculation_shift   128
#define infimum                   infimum_syntectic_1
#include "amaru/infimum.h"
#undef AMARU_AMAHU_INFIMUM_H_
#undef amaru_calculation_infimum
#undef amaru_multiply_type
#undef amaru_size
#undef amaru_limb1_t
#undef amaru_calculation_shift
#undef infimum

// For amaru_calculation_infimum == amaru_built_in_1
#define amaru_calculation_infimum amaru_built_in_1
#define amaru_multiply_type       amaru_built_in_1
#define amaru_size                64
#define amaru_limb1_t             std::uint64_t
#define amaru_calculation_shift   128
#define infimum                   infimum_built_in_1
#include "amaru/infimum.h"
#undef AMARU_AMAHU_INFIMUM_H_
#undef amaru_calculation_infimum
#undef amaru_multiply_type
#undef amaru_size
#undef amaru_limb1_t
#undef amaru_calculation_shift
#undef infimum

#include <gtest/gtest.h>

#include <cstdint>

TEST(infimum, built_in_4) {
  EXPECT_EQ(infimum_built_in_4(0xffff, 0xffff, 0x0fff),
    0xfffe);
}

TEST(infimum, syntectic_2) {
  EXPECT_EQ( infimum_syntectic_2(0xffffffff, 0xffffffff, 0x0fffffff),
    0xfffffffe);
}

TEST(infimum, built_in_2) {
  EXPECT_EQ(infimum_built_in_2(0xffffffff, 0xffffffff, 0x0fffffff),
    0xfffffffe);
}

TEST(infimum, syntectic_1) {
  EXPECT_EQ(
    infimum_syntectic_1(0xffffffffffffffff, 0xffffffffffffffff, 0x0fffffffffffffff),
    0xfffffffffffffffe);
}

TEST(infimum, infimum_built_in_1) {
  EXPECT_EQ(
    infimum_syntectic_1(0xffffffffffffffff, 0xffffffffffffffff, 0x0fffffffffffffff),
    0xfffffffffffffffe);
}
