// gcc -O3 -std=c11 -I. -I./include -I ~/ryu/cassio/ryu -c generated/ieee_float.c -Wall -Wextra
// g++ -O3 -std=c++11 -I. -I./include -I ~/ryu/cassio/ryu -c generated/ieee_float.c -Wall -Wextra

// g++ -O3 -std=c++11 -I. -I./include -I ~/ryu/cassio/ryu -c tests/test.cpp -Wall -Wextra
// g++ -o test test.o ieee_float.o ~/ryu/cassio/ryu/libryu.a -lgtest -lgtest_main

#define DO_RYU   1
#define DO_AMARU 1

#include "common.h"
#include "ieee.h"

#include <ryu.h>

#include <gtest/gtest.h>

#include <cstdint>
#include <cmath>

namespace {

struct float_params_t {
  static auto constexpr exponent_size  = uint32_t{8};
  static auto constexpr mantissa_size  = uint32_t{23};
  static auto constexpr large_exponent = uint32_t{10};
  static auto constexpr word_size      = uint32_t{32};
  static auto constexpr exponent_min   = int32_t{-149};
  static auto constexpr mantissa_max   = AMARU_POW2(uint32_t, mantissa_size);
};

static inline
float ieee_float_to_value(ieee_float_t const ieee) {
  uint32_t uint;
  uint   = ieee.negative;
  uint <<= float_params_t::exponent_size;
  uint  |= ieee.exponent;
  uint <<= float_params_t::mantissa_size;
  uint  |= ieee.mantissa;
  float value;
  memcpy(&value, &uint, sizeof(uint));
  return value;
}

} // namespace <anonymous>

struct float_tests : testing::TestWithParam<int32_t> {
};


TEST_P(float_tests, test_all_mantissas)
{
  auto const exponent = GetParam();
  auto       ieee     = ieee_float_t{false, exponent, exponent == 0 };

  while (ieee.mantissa != float_params_t::mantissa_max) {

    #if DO_RYU
      auto ryu = f2d(ieee.mantissa, ieee.exponent);
      //asm("" : "+r"(ieee.mantissa));
    #endif
    #if DO_AMARU
      auto const value = ieee_float_to_value(ieee);
      auto amaru = amaru_ieee_float_from_value(value);
      //asm("" : "+r"(amaru.mantissa));
    #endif

    #if DO_RYU && DO_AMARU
//      ASSERT_EQ(ryu.exponent, amaru.exponent) <<
//        "Note: ieee.mantissa = " << ieee.mantissa << ", "
//        "value = " << value;

      ASSERT_EQ(ryu.mantissa, amaru.mantissa) <<
        "Note: ieee.mantissa = " << ieee.mantissa << ", "
        "value = " << value;
    #endif

    ++ieee.mantissa;
  }
}

INSTANTIATE_TEST_SUITE_P(all_exponents, float_tests, \
    testing::Range(0, 254));

