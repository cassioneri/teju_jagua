#ifndef AMARU_CPP_TEST_IMPL_HPP
#define AMARU_CPP_TEST_IMPL_HPP

#include <cstdint>

// Common macros required by amaru/div10.h and amaru/mshift.h.
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

struct built_in_1_small_shift_t {

  static int const shift;

  static amaru_u1_t
  div10(amaru_u1_t const m);

  static amaru_u1_t
  mshift(amaru_u1_t const m, amaru_u1_t const u, amaru_u1_t const l);

};

struct synthetic_1_t {

  static int const shift;

  static amaru_u1_t
  div10(amaru_u1_t const m);

  static amaru_u1_t
  mshift(amaru_u1_t const m, amaru_u1_t const u, amaru_u1_t const l);

};

struct synthetic_1_small_shift_t {

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

struct synthetic_2_t {

  static int const shift;

  static amaru_u1_t
  mshift(amaru_u1_t const m, amaru_u1_t const u, amaru_u1_t const l);

};

struct synthetic_2_small_shift_t {

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

#endif // AMARU_CPP_TEST_IMPL_HPP