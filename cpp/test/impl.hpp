/**
 * @file cpp/test/impl.hpp
 *
 * Several implementations of amaru_div10 and amaru_mshift used for testing and
 * based on different platform capabilities.
 */

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

  /**
   * @brief Implementation of amaru_div10 for amaru_calculation_div10 =
   * amaru_built_in_1.
   *
   * @param m               Forwarded to amaru_div10.
   */
  static amaru_u1_t
  div10(amaru_u1_t m);

  /**
   * @brief Implementation of amaru_mshift for amaru_calculation_shift =
   * amaru_built_in_1 and amaru_calculation_shift >= 2 * amaru_size.
   *
   * @param m               Forwarded to amaru_mshift.
   * @param u               Forwarded to amaru_mshift.
   * @param l               Forwarded to amaru_mshift.
   */
  static amaru_u1_t
  mshift(amaru_u1_t m, amaru_u1_t u, amaru_u1_t l);

};

struct built_in_1_small_shift_t {

  static int const shift;

  /**
   * @brief Implementation of amaru_mshift for amaru_calculation_shift =
   * amaru_built_in_1 and amaru_calculation_shift < 2 * amaru_size.
   *
   * @param m               Forwarded to amaru_mshift.
   * @param u               Forwarded to amaru_mshift.
   * @param l               Forwarded to amaru_mshift.
   */
  static amaru_u1_t
  mshift(amaru_u1_t m, amaru_u1_t u, amaru_u1_t l);

};

struct synthetic_1_t {

  static int const shift;

  /**
   * @brief Implementation of amaru_div10 for amaru_calculation_div10 =
   * amaru_synthetic_1.
   *
   * @param m               Forwarded to amaru_div10.
   */
  static amaru_u1_t
  div10(amaru_u1_t const m);

  /**
   * @brief Implementation of amaru_mshift for amaru_calculation_shift =
   * amaru_synthetic_1 and amaru_calculation_shift >= 2 * amaru_size.
   *
   * @param m               Forwarded to amaru_mshift.
   * @param u               Forwarded to amaru_mshift.
   * @param l               Forwarded to amaru_mshift.
   */
  static amaru_u1_t
  mshift(amaru_u1_t m, amaru_u1_t u, amaru_u1_t l);

};

struct synthetic_1_small_shift_t {

  static int const shift;

  /**
   * @brief Implementation of amaru_mshift for amaru_calculation_shift =
   * amaru_synthetic_1 and amaru_calculation_shift < 2 * amaru_size.
   *
   * @param m               Forwarded to amaru_mshift.
   * @param u               Forwarded to amaru_mshift.
   * @param l               Forwarded to amaru_mshift.
   */
  static amaru_u1_t
  mshift(amaru_u1_t m, amaru_u1_t u, amaru_u1_t l);

};

struct built_in_2_t {

  static int const shift;

  /**
   * @brief Implementation of div10 for amaru_calculation_div10 =
   * amaru_built_in_2.
   *
   * @param m               Forwarded to amaru_div10.
   */
  static amaru_u1_t
  div10(amaru_u1_t m);

  /**
   * @brief Implementation of amaru_mshift for amaru_calculation_shift =
   * amaru_built_in_2.
   *
   * @param m               Forwarded to amaru_mshift.
   * @param u               Forwarded to amaru_mshift.
   * @param l               Forwarded to amaru_mshift.
   */
  static amaru_u1_t
  mshift(amaru_u1_t m, amaru_u1_t u, amaru_u1_t l);

};

struct synthetic_2_t {

  static int const shift;

  /**
   * @brief Implementation of amaru_mshift for amaru_calculation_shift =
   * amaru_synthetic_2 and amaru_calculation_shift >= 2 * amaru_size.
   *
   * @param m               Forwarded to amaru_mshift.
   * @param u               Forwarded to amaru_mshift.
   * @param l               Forwarded to amaru_mshift.
   */
  static amaru_u1_t
  mshift(amaru_u1_t m, amaru_u1_t u, amaru_u1_t l);

};

struct synthetic_2_small_shift_t {

  static int const shift;

  /**
   * @brief Implementation of amaru_mshift for amaru_calculation_shift =
   * amaru_synthetic_2 and amaru_calculation_shift < 2 * amaru_size.
   *
   * @param m               Forwarded to amaru_mshift.
   * @param u               Forwarded to amaru_mshift.
   * @param l               Forwarded to amaru_mshift.
   */
  static amaru_u1_t
  mshift(amaru_u1_t m, amaru_u1_t u, amaru_u1_t l);

};

struct built_in_4_t {

  static int const shift;

  /**
   * @brief Implementation of amaru_mshift for amaru_calculation_shift =
   * amaru_built_in_4.
   *
   * @param m               Forwarded to amaru_mshift.
   * @param u               Forwarded to amaru_mshift.
   * @param l               Forwarded to amaru_mshift.
   */
  static amaru_u1_t
  mshift(amaru_u1_t m, amaru_u1_t u, amaru_u1_t l);

};

} // namespace test
} // namespace amaru

#endif // AMARU_CPP_TEST_IMPL_HPP
