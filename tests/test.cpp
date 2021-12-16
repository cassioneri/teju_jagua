// gcc -O3 -std=c11 -I. -I./include -I ~/ryu/cassio/ryu -c generated/ieee32.c -Wall -Wextra
// g++ -O3 -std=c++11 -I. -I./include -I ~/ryu/cassio/ryu -c generated/ieee32.c -Wall -Wextra

// g++ -O3 -std=c++11 -I. -I./include -I ~/ryu/cassio/ryu -c tests/test.cpp -Wall -Wextra
// g++ -o test test.o ieee32.o ~/ryu/cassio/ryu/libryu.a -lgtest -lgtest_main

#define DO_RYU   1
#define DO_AMARU 1

#include "common.h"
#include "ieee.h"

#include <ryu.h>

#include <gtest/gtest.h>

#include <cstdint>
#include <cmath>

namespace {

struct ieee32_params_t {
  static auto constexpr exponent_size = uint32_t{8};
  static auto constexpr exponent_max  = int32_t{255};
  static auto constexpr mantissa_size = uint32_t{23};
  static auto constexpr mantissa_max  = AMARU_POW2(uint32_t, mantissa_size);
};

float to_value(ieee32_t const ieee) {
  uint32_t uint;
  uint   = ieee.negative;
  uint <<= ieee32_params_t::exponent_size;
  uint  |= ieee.exponent;
  uint <<= ieee32_params_t::mantissa_size;
  uint  |= ieee.mantissa;
  float value;
  memcpy(&value, &uint, sizeof(uint));
  return value;
}

} // namespace <anonymous>

TEST(amaru_tests, test_all_floats_against_ryu)
{

  auto ieee = ieee32_t{false, 0, 1};

  for ( ; ieee.exponent < ieee32_params_t::exponent_max; ++ieee.exponent) {

    #if DO_RYU && DO_AMARU
      std::cerr << "Exponent: " << ieee.exponent << std::endl;
    #endif

    for (ieee.mantissa = ieee.exponent == 0;
      ieee.mantissa < ieee32_params_t::mantissa_max; ++ieee.mantissa) {

      #if DO_RYU
        auto ryu = f2d(ieee.mantissa, ieee.exponent);
        (void) ryu;
      #endif

      #if DO_AMARU
        auto const value = to_value(ieee);
        auto amaru = amaru_float(value);
        (void) amaru;
      #else
        // Silence -Wunused-function
        (void) to_value;
      #endif

      #if DO_RYU && DO_AMARU
        ASSERT_EQ(ryu.exponent, amaru.exponent) <<
          "Note: ieee.mantissa = " << ieee.mantissa << ", "
          "value = " << value;

        ASSERT_EQ(ryu.mantissa, amaru.mantissa) <<
          "Note: ieee.mantissa = " << ieee.mantissa << ", "
          "value = " << value;
      #endif
    }
  }
}
