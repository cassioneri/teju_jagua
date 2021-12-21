/*
ieee32.c in C and C++
  gcc -O3 -std=c11 -I. -I./include -c generated/ieee32.c -Wall -Wextra
  g++ -O3 -std=c++11 -I. -I./include -c generated/ieee32.c -Wall -Wextra

ieee64.c in C and C++
  gcc -O3 -std=c11 -I. -I./include -c generated/ieee64.c -Wall -Wextra
  g++ -O3 -std=c++11 -I. -I./include -c generated/ieee64.c -Wall -Wextra

test.cpp
  g++ -O3 -std=c++11 -o test -I. -I./include -I ~/ryu/cassio/ryu src/test.cpp -Wall -Wextra ieee32.o ieee64.o ~/ryu/cassio/ryu/libryu.a -lgtest -lgtest_main
 */

#define DO_RYU   1
#define DO_AMARU 1

#include "common.h"
#include "ieee.h"

#include <ryu.h>

#include <gtest/gtest.h>

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_bin_float.hpp>

#include <cstdint>
#include <random>
#include <cstring>

namespace {

using mp_float_t = boost::multiprecision::cpp_bin_float_50;
using mp_int_t   = boost::multiprecision::cpp_int;

template <typename T>
struct log_traits_t;

template <>
struct log_traits_t<int32_t> {
  using                 sint = int32_t;
  using                 dint = int64_t;
  static auto constexpr size = 32;
};

template <>
struct log_traits_t<int64_t> {
  using                 sint = int64_t;
  using                 dint = __int128_t;
  static auto constexpr size = 64;
};

template <typename T>
typename log_traits_t<T>::sint get_multiplier(mp_float_t const& log) {
  using      traits_t   = log_traits_t<T>;
  auto const pow2size   = pow(mp_float_t{2.}, traits_t::size);
  auto const multiplier = static_cast<typename traits_t::sint>(log * pow2size);
  return multiplier;
}

template <typename T>
void test_log(typename log_traits_t<T>::sint multiplier,
  typename log_traits_t<T>::sint min, typename log_traits_t<T>::sint max) {

  using traits_t = log_traits_t<T>;

  auto const lower = static_cast<typename traits_t::dint>(multiplier);
  auto const upper = lower + 1;

  for (typename traits_t::sint n = 0; n >= min; --n) {
    auto const lower_bound = lower * n >> traits_t::size;
    auto const upper_bound = upper * n >> traits_t::size;
    ASSERT_EQ(lower_bound, upper_bound) << "Note n = " << n;
  }

  for (typename traits_t::sint n = 0; n < max; ++n) {
    auto const lower_bound = lower * n >> traits_t::size;
    auto const upper_bound = upper * n >> traits_t::size;
    ASSERT_EQ(lower_bound, upper_bound) << "Note n = " << n;
  }

  {
    auto const n = min - 1;
    auto const lower_bound = lower * n >> traits_t::size;
    auto const upper_bound = upper * n >> traits_t::size;
    EXPECT_GT(lower_bound, upper_bound) << "Minimum " << min << " isn't sharp.";
  }

  {
    auto const n = max;
    auto const lower_bound = lower * n >> traits_t::size;
    auto const upper_bound = upper * n >> traits_t::size;
    EXPECT_LT(lower_bound, upper_bound) << "Maximum " << max << " isn't sharp.";
  }
}

auto const log10_2 = mp_float_t{".30102999566398119521373889472449302676818988146210"};

TEST(log10_pow2_tests, for_int32_t) {
  auto const multiplier = get_multiplier<int32_t>(log10_2);
  EXPECT_EQ(multiplier, int32_t{1292913986});
  test_log<int32_t>(multiplier, int32_t{-70776}, int32_t{70777});
}

TEST(log10_pow2_tests, for_int64_t) {
  auto const multiplier = get_multiplier<int64_t>(log10_2);
  EXPECT_EQ(multiplier, int64_t{5553023288523357132});
  test_log<int64_t>(multiplier, int64_t{-1923400329}, int64_t{1923400330});
}

auto const log5_2  = mp_float_t{".43067655807339305067010656876396563206979193207975"};

TEST(log5_pow2_tests, for_int32_t) {
  auto const multiplier = get_multiplier<int32_t>(log5_2);
  EXPECT_EQ(multiplier, int32_t{1849741732});
  test_log<int32_t>(multiplier, int32_t{-78854}, int32_t{78855});
}

TEST(log5_pow2_tests, for_int64_t) {
  auto const multiplier = get_multiplier<int64_t>(log5_2);
  EXPECT_EQ(multiplier, int64_t{7944580245325990804});
  test_log<int64_t>(multiplier, int64_t{-1344399136}, int64_t{1344399137});
}

template <typename>
struct fp_traits_t;

template <typename T>
typename fp_traits_t<T>::fp_t
to_value(typename fp_traits_t<T>::rep_t const& ieee) {
  typename fp_traits_t<T>::suint_t uint;
  uint   = ieee.negative;
  uint <<= fp_traits_t<T>::exponent_size;
  uint  |= ieee.exponent;
  uint <<= fp_traits_t<T>::mantissa_size;
  uint  |= ieee.mantissa;
  typename fp_traits_t<T>::fp_t value;
  std::memcpy(&value, &uint, sizeof(uint));
  return value;
}

template <>
struct fp_traits_t<float> {

  using fp_t    = float;
  using suint_t = uint32_t;
  using rep_t   = ieee32_t;

  static auto constexpr exponent_size = uint32_t{8};
  static auto constexpr mantissa_size = uint32_t{23};

  static auto constexpr exponent_max  = AMARU_POW2(int32_t, exponent_size) - 1;
  static auto constexpr mantissa_max  = AMARU_POW2(suint_t, mantissa_size);

  static rep_t
  to_ieee(fp_t value) {
    return to_ieee32(value);
  }

  static rep_t
  ryu(rep_t const& ieee) {
    auto const ryu = f2d(ieee.mantissa, ieee.exponent);
    return {false, ryu.exponent, ryu.mantissa};
  }

  static rep_t
  amaru(rep_t const& ieee) {
    return amaru_float(to_value<float>(ieee));
  }
};

template <>
struct fp_traits_t<double> {

  using fp_t    = double;
  using suint_t = uint64_t;
  using rep_t   = ieee64_t;

  static auto constexpr exponent_size = uint32_t{11};
  static auto constexpr mantissa_size = uint32_t{52};
  static auto constexpr exponent_max  = AMARU_POW2(int32_t, exponent_size) - 1;
  static auto constexpr mantissa_max  = AMARU_POW2(suint_t, mantissa_size);

  static rep_t
  to_ieee(fp_t value) {
    return to_ieee64(value);
  }

  static rep_t
  ryu(rep_t const& ieee) {
    auto const ryu = d2d(ieee.mantissa, ieee.exponent);
    return {false, ryu.exponent, ryu.mantissa};
  }

  static rep_t
  amaru(rep_t const& ieee) {
    return amaru_double(to_value<double>(ieee));
  }
};

template <typename T>
void compare_to_ryu(typename fp_traits_t<T>::rep_t const& ieee) {

  using traits_t = fp_traits_t<T>;

  #if DO_RYU
    auto const ryu_dec = traits_t::ryu(ieee);
    (void) ryu_dec;
  #endif

  #if DO_AMARU
    auto const amaru_dec = traits_t::amaru(ieee);
    (void) amaru_dec;
  #endif

  #if DO_RYU && DO_AMARU
    EXPECT_EQ(ryu_dec.exponent, amaru_dec.exponent) << "Note: "
      "ieee.exponent = " << ieee.exponent << ", "
      "ieee.mantissa = " << ieee.mantissa;

    EXPECT_EQ(ryu_dec.mantissa, amaru_dec.mantissa) << "Note: "
        "ieee.exponent = " << ieee.exponent << ", "
        "ieee.mantissa = " << ieee.mantissa;
  #endif
}

TEST(float_tests, exhaustive_comparison_to_ryu) {

  using traits_t = fp_traits_t<float>;

  for (int32_t exponent = 0; !HasFailure() && exponent < traits_t::exponent_max;
    ++exponent) {

    #if DO_RYU && DO_AMARU
      std::cerr << "Exponent: " << exponent << std::endl;
    #endif

    for (traits_t::suint_t mantissa = exponent == 0;
      mantissa < traits_t::mantissa_max; ++mantissa) {

      auto const ieee = traits_t::rep_t{false, exponent, mantissa};
      compare_to_ryu<traits_t::fp_t>(ieee);
    }
  }
}

TEST(double_tests, random_comparison_to_ryu) {

  using traits_t = fp_traits_t<double>;

  // Ad hoc test
  if (false) {
    compare_to_ryu<traits_t::fp_t>({false, 582, 3372137371404177});
    return;
  }

  traits_t::suint_t uint_max;
  auto const fp_max = std::numeric_limits<traits_t::fp_t>::max();
  memcpy(&uint_max, &fp_max, sizeof(fp_max));

  std::random_device rd;
  auto dist = std::uniform_int_distribution<traits_t::suint_t>{1, uint_max};

  auto number_of_tests = uint32_t{100000000};

  // Using the "downto" operator :-D
  // https://stackoverflow.com/questions/1642028/what-is-the-operator-in-c-c
  while (!HasFailure() && number_of_tests --> 0) {
    auto const i = dist(rd);
    traits_t::fp_t x;
    memcpy(&x, &i, sizeof(i));
    auto const ieee = traits_t::to_ieee(x);
    compare_to_ryu<traits_t::fp_t>(ieee);
    if (HasFailure())
      FAIL() << x;
  }
}

} // namespace <anonymous>
