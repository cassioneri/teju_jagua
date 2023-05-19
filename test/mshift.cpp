#include "amaru/config.h"
#include "test/multiply.h"

#include <cstdint>

#include <gtest/gtest.h>

// For amaru_calculation_mshift == amaru_built_in_4
#define amaru_calculation_mshift amaru_built_in_4
#define amaru_multiply_type      amaru_built_in_4
#define amaru_size               16
#define amaru_u1_t               std::uint16_t
#define amaru_u2_t               std::uint32_t
#define amaru_u4_t               std::uint64_t
#define amaru_calculation_shift  32
#define amaru_rshift             amaru_rshift_built_in_4
#define amaru_mshift             amaru_mshift_built_in_4
#include "../amaru/mshift.h"
#undef AMARU_AMARU_MSHIFT_H_
#undef amaru_rshift
#undef amaru_mshift
#undef amaru_calculation_mshift
#undef amaru_multiply_type
#undef amaru_size
#undef amaru_u1_t
#undef amaru_u2_t
#undef amaru_u4_t
#undef amaru_calculation_shift
#undef amaru_id

// For amaru_calculation_mshift == amaru_syntectic_2
#define amaru_calculation_mshift amaru_syntectic_2
#define amaru_multiply_type      amaru_syntectic_2
#define amaru_size               32
#define amaru_u1_t               std::uint32_t
#define amaru_u2_t               std::uint64_t
#define amaru_calculation_shift  64
#define amaru_rshift             amaru_rshift_syntectic_2
#define amaru_mshift             amaru_mshift_syntectic_2
#include "../amaru/mshift.h"
#undef AMARU_AMARU_MSHIFT_H_
#undef amaru_rshift
#undef amaru_mshift
#undef amaru_calculation_mshift
#undef amaru_multiply_type
#undef amaru_size
#undef amaru_u1_t
#undef amaru_u2_t
#undef amaru_calculation_shift
#undef amaru_id

// For amaru_calculation_mshift == amaru_built_in_2
#define amaru_calculation_mshift amaru_built_in_2
#define amaru_multiply_type      amaru_built_in_2
#define amaru_size               32
#define amaru_u1_t               std::uint32_t
#define amaru_u2_t               std::uint64_t
#define amaru_calculation_shift  64
#define amaru_rshift             amaru_rshift_built_in_2
#define amaru_mshift             amaru_mshift_built_in_2
#include "../amaru/mshift.h"
#undef AMARU_AMARU_MSHIFT_H_
#undef amaru_rshift
#undef amaru_mshift
#undef amaru_calculation_mshift
#undef amaru_multiply_type
#undef amaru_size
#undef amaru_u1_t
#undef amaru_u2_t
#undef amaru_calculation_shift
#undef amaru_id

// For amaru_calculation_mshift == amaru_syntectic_1
#define amaru_calculation_mshift amaru_syntectic_1
#define amaru_multiply_type      amaru_syntectic_1
#define amaru_size               64
#define amaru_u1_t               std::uint64_t
#define amaru_calculation_shift  128
#define amaru_rshift             amaru_rshift_syntectic_1
#define amaru_mshift             amaru_mshift_syntectic_1
#include "../amaru/mshift.h"
#undef amaru_rshift
#undef amaru_mshift
#undef AMARU_AMARU_MSHIFT_H_
#undef amaru_calculation_mshift
#undef amaru_multiply_type
#undef amaru_size
#undef amaru_u1_t
#undef amaru_calculation_shift
#undef amaru_id

// For amaru_calculation_mshift == amaru_built_in_1
#define amaru_calculation_mshift amaru_built_in_1
#define amaru_multiply_type      amaru_built_in_1
#define amaru_size               64
#define amaru_u1_t               std::uint64_t
#define amaru_calculation_shift  128
#define amaru_rshift             amaru_rshift_built_in_1
#define amaru_mshift             amaru_mshift_built_in_1
#include "../amaru/mshift.h"
#undef amaru_rshift
#undef amaru_mshift
#undef AMARU_AMARU_MSHIFT_H_
#undef amaru_calculation_mshift
#undef amaru_multiply_type
#undef amaru_size
#undef amaru_u1_t
#undef amaru_calculation_shift
#undef amaru_id

TEST(mshift, built_in_4) {
  EXPECT_EQ(amaru_mshift_built_in_4(0xffff, 0xffff, 0x0fff), 0xfffe);
}

TEST(mshift, syntectic_2) {
  EXPECT_EQ(amaru_mshift_syntectic_2(0xffffffff, 0xffffffff, 0x0fffffff),
    0xfffffffe);
}

TEST(mshift, built_in_2) {
  EXPECT_EQ(amaru_mshift_built_in_2(0xffffffff, 0xffffffff, 0x0fffffff),
    0xfffffffe);
}

TEST(mshift, syntectic_1) {
  EXPECT_EQ(
    amaru_mshift_syntectic_1(0xffffffffffffffff, 0xffffffffffffffff,
    0x0fffffffffffffff), 0xfffffffffffffffe);
}

TEST(mshift, built_in_1) {
  EXPECT_EQ(amaru_mshift_built_in_1(0xffffffffffffffff, 0xffffffffffffffff,
    0xffffffffffffffff), 0xfffffffffffffffe);
}
