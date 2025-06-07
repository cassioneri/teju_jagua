// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

/**
 * @file cpp/test/impl.hpp
 *
 * Several implementations of teju_div10 and teju_mshift used for testing and
 * based on different platform capabilities.
 */

#ifndef TEJU_CPP_TEST_IMPL_HPP
#define TEJU_CPP_TEST_IMPL_HPP

#include <cstdint>

// Common macros required by teju/div10.h and teju/mshift.h.
#define teju_size   16
#define teju_u1_t   std::uint16_t
#define teju_u2_t   std::uint32_t
#define teju_u4_t   std::uint64_t

typedef struct {
  teju_u1_t const upper;
  teju_u1_t const lower;
} teju_multiplier_t;

namespace teju {
namespace test {

struct built_in_1_t {

  static int const shift;

  /**
   * @brief Implementation of teju_mshift for teju_calculation_shift =
   *        teju_built_in_1 and teju_calculation_shift >= 2 * teju_size.
   *
   * @param  m              The multiplicand m.
   * @param  M              The multiplicand M.
   *
   * @returns teju_mshift(m, M);
   */
  static teju_u1_t
  mshift(teju_u1_t m, teju_multiplier_t M);

};

struct built_in_1_small_shift_t {

  static int const shift;

  /**
   * @brief Implementation of teju_mshift for teju_calculation_shift =
   *        teju_built_in_1 and teju_calculation_shift < 2 * teju_size.
   *
   * @param  m              The multiplicand m.
   * @param  M              The multiplicand M.
   *
   * @returns teju_mshift(m, M);
   */
  static teju_u1_t
  mshift(teju_u1_t m, teju_multiplier_t M);

};

struct synthetic_1_t {

  static int const shift;

  /**
   * @brief Implementation of teju_div10 for teju_calculation_div10 =
   *        teju_synthetic_1.
   *
   * @param  m              Forwarded to teju_div10.
   *
   * @returns teju_div10(m).
   */
  static teju_u1_t
  div10(teju_u1_t const m);

  /**
   * @brief Implementation of teju_mshift for teju_calculation_shift =
   *        teju_synthetic_1 and teju_calculation_shift >= 2 * teju_size.
   *
   * @param  m              Forwarded to teju_mshift.
   * @param  m              The multiplicand m.
   * @param  M              The multiplicand M.
   *
   * @returns teju_mshift(m, M);
   */
  static teju_u1_t
  mshift(teju_u1_t m, teju_multiplier_t M);

};

struct synthetic_1_small_shift_t {

  static int const shift;

  /**
   * @brief Implementation of teju_mshift for teju_calculation_shift =
   *        teju_synthetic_1 and teju_calculation_shift < 2 * teju_size.
   *
   * @param  m              The multiplicand m.
   * @param  M              The multiplicand M.
   *
   * @returns teju_mshift(m, M);
   */
  static teju_u1_t
  mshift(teju_u1_t m, teju_multiplier_t M);

};

struct built_in_2_t {

  static int const shift;

  /**
   * @brief Implementation of div10 for teju_calculation_div10 =
   *        teju_built_in_2.
   *
   * @param  m              Forwarded to teju_div10.
   *
   * @returns teju_div10(m).
   */
  static teju_u1_t
  div10(teju_u1_t m);

  /**
   * @brief Implementation of teju_mshift for teju_calculation_shift =
   *        teju_built_in_2.
   *
   * @param  m              The multiplicand m.
   * @param  M              The multiplicand M.
   *
   * @returns teju_mshift(m, M);
   */
  static teju_u1_t
  mshift(teju_u1_t m, teju_multiplier_t M);

};

struct synthetic_2_t {

  static int const shift;

  /**
   * @brief Implementation of teju_mshift for teju_calculation_shift =
   *        teju_synthetic_2 and teju_calculation_shift >= 2 * teju_size.
   *
   * @param  m              The multiplicand m.
   * @param  M              The multiplicand M.
   *
   * @returns teju_mshift(m, M);
   */
  static teju_u1_t
  mshift(teju_u1_t m, teju_multiplier_t M);

};

struct synthetic_2_small_shift_t {

  static int const shift;

  /**
   * @brief Implementation of teju_mshift for teju_calculation_shift =
   *        teju_synthetic_2 and teju_calculation_shift < 2 * teju_size.
   *
   * @param  m              The multiplicand m.
   * @param  M              The multiplicand M.
   *
   * @returns teju_mshift(m, M);
   */
  static teju_u1_t
  mshift(teju_u1_t m, teju_multiplier_t M);

};

struct built_in_4_t {

  static int const shift;

  /**
   * @brief Implementation of teju_mshift for teju_calculation_shift =
   *        teju_built_in_4.
   *
   * @param  m              The multiplicand m.
   * @param  M              The multiplicand M.
   *
   * @returns teju_mshift(m, M);
   */
  static teju_u1_t
  mshift(teju_u1_t m, teju_multiplier_t M);

};

} // namespace test
} // namespace teju

#endif // TEJU_CPP_TEST_IMPL_HPP
