#ifndef AMARU_TEST_IMPL_HPP
#define AMARU_TEST_IMPL_HPP

#include "amaru/config.h"

#include <cstdint>

#define amaru_size 16
#define amaru_u1_t std::uint16_t
#define amaru_u2_t std::uint32_t
#define amaru_u4_t std::uint64_t

namespace amaru {
namespace test {

struct built_in_1_t {

  static int const shift;

  static amaru_u1_t
  div10(amaru_u1_t const m);

  static amaru_u1_t
  mshift(amaru_u1_t const m, amaru_u1_t const u, amaru_u1_t const l);

};

struct syntectic_1_t {

  static int const shift;

  static amaru_u1_t
  div10(amaru_u1_t const m);

  static amaru_u1_t
  mshift(amaru_u1_t const m, amaru_u1_t const u, amaru_u1_t const l);

};

struct syntectic_1_small_shift_t {

  static int const shift;

  static amaru_u1_t
  mshift(amaru_u1_t const m, amaru_u1_t const u, amaru_u1_t const l);

};

struct built_in_2_t {

  static int const shift;

  static amaru_u1_t
  div10(amaru_u1_t const m);

  static amaru_u1_t
  mshift(amaru_u1_t const m, amaru_u1_t const u, amaru_u1_t const l);

};

struct syntectic_2_t {

  static int const shift;

  static amaru_u1_t
  mshift(amaru_u1_t const m, amaru_u1_t const u, amaru_u1_t const l);

};

struct syntectic_2_small_shift_t {

  static int const shift;

  static amaru_u1_t
  mshift(amaru_u1_t const m, amaru_u1_t const u, amaru_u1_t const l);

};

struct built_in_4_t {

  static int const shift;

  static amaru_u1_t
  mshift(amaru_u1_t const m, amaru_u1_t const u, amaru_u1_t const l);

};

} // namespace test
} // namespace amaru

#endif // AMARU_TEST_IMPL_HPP
