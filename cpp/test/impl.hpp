/**
 * @file cpp/test/impl.hpp
 *
 * Several implementations of \c amaru_div10 and \c amaru_mshift used for
 * testing and based on different platform capabilities.
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
   * @brief Implementation of \c amaru_div10 for
   * <tt>amaru_calculation_div10 == amaru_built_in_1</tt>.
   * 
   * @param m               Forwarded to \c amaru_div10.
   */
  static amaru_u1_t
  div10(amaru_u1_t m);

  /**
   * @brief Implementation of \c amaru_mshift for
   * <tt>amaru_calculation_shift == amaru_built_in_1</tt> and 
   * <tt>amaru_calculation_shift >= 2 * amaru_size</tt>.
   * 
   * @param m               Forwarded to \c amaru_mshift.
   * @param u               Forwarded to \c amaru_mshift.
   * @param l               Forwarded to \c amaru_mshift.
   */
  static amaru_u1_t
  mshift(amaru_u1_t m, amaru_u1_t u, amaru_u1_t l);

};

struct built_in_1_small_shift_t {

  static int const shift;

  /**
   * @brief Implementation of \c amaru_mshift for
   * <tt>amaru_calculation_shift == amaru_built_in_1</tt> and 
   * <tt>amaru_calculation_shift < 2 * amaru_size</tt>.
   * 
   * @param m               Forwarded to \c amaru_mshift.
   * @param u               Forwarded to \c amaru_mshift.
   * @param l               Forwarded to \c amaru_mshift.
   */
  static amaru_u1_t
  mshift(amaru_u1_t m, amaru_u1_t u, amaru_u1_t l);

};

struct synthetic_1_t {

  static int const shift;

  /**
   * @brief Implementation of \c amaru_div10 for
   * <tt>amaru_calculation_div10 == amaru_synthetic_1</tt>.
   * 
   * @param m               Forwarded to \c amaru_div10.
   */
  static amaru_u1_t
  div10(amaru_u1_t const m);

  /**
   * @brief Implementation of \c amaru_mshift for
   * <tt>amaru_calculation_shift == amaru_synthetic_1</tt> and 
   * <tt>amaru_calculation_shift >= 2 * amaru_size</tt>.
   * 
   * @param m               Forwarded to \c amaru_mshift.
   * @param u               Forwarded to \c amaru_mshift.
   * @param l               Forwarded to \c amaru_mshift.
   */
  static amaru_u1_t
  mshift(amaru_u1_t m, amaru_u1_t u, amaru_u1_t l);

};

struct synthetic_1_small_shift_t {

  static int const shift;

  /**
   * @brief Implementation of \c amaru_mshift for
   * <tt>amaru_calculation_shift == amaru_synthetic_1</tt> and 
   * <tt>amaru_calculation_shift < 2 * amaru_size</tt>.
   * 
   * @param m               Forwarded to \c amaru_mshift.
   * @param u               Forwarded to \c amaru_mshift.
   * @param l               Forwarded to \c amaru_mshift.
   */
  static amaru_u1_t
  mshift(amaru_u1_t m, amaru_u1_t u, amaru_u1_t l);

};

struct built_in_2_t {

  static int const shift;

  /**
   * @brief Implementation of \c div10 for
   * <tt>amaru_calculation_div10 == amaru_built_in_2</tt>.
   * 
   * @param m               Forwarded to \c amaru_div10.
   */
  static amaru_u1_t
  div10(amaru_u1_t m);

  /**
   * @brief Implementation of \c amaru_mshift for
   * <tt>amaru_calculation_shift == amaru_built_in_2</tt>.
   * 
   * @param m               Forwarded to \c amaru_mshift.
   * @param u               Forwarded to \c amaru_mshift.
   * @param l               Forwarded to \c amaru_mshift.
   */
  static amaru_u1_t
  mshift(amaru_u1_t m, amaru_u1_t u, amaru_u1_t l);

};

struct synthetic_2_t {

  static int const shift;

  /**
   * @brief Implementation of \c amaru_mshift for
   * <tt>amaru_calculation_shift == amaru_synthetic_2</tt> and 
   * <tt>amaru_calculation_shift >= 2 * amaru_size</tt>.
   * 
   * @param m               Forwarded to \c amaru_mshift.
   * @param u               Forwarded to \c amaru_mshift.
   * @param l               Forwarded to \c amaru_mshift.
   */
  static amaru_u1_t
  mshift(amaru_u1_t m, amaru_u1_t u, amaru_u1_t l);

};

struct synthetic_2_small_shift_t {

  static int const shift;

  /**
   * @brief Implementation of \c amaru_mshift for
   * <tt>amaru_calculation_shift == amaru_synthetic_2</tt> and 
   * <tt>amaru_calculation_shift < 2 * amaru_size</tt>.
   * 
   * @param m               Forwarded to \c amaru_mshift.
   * @param u               Forwarded to \c amaru_mshift.
   * @param l               Forwarded to \c amaru_mshift.
   */
  static amaru_u1_t
  mshift(amaru_u1_t m, amaru_u1_t u, amaru_u1_t l);

};

struct built_in_4_t {

  static int const shift;

  /**
   * @brief Implementation of \c amaru_mshift for
   * <tt>amaru_calculation_shift == amaru_built_in_4</tt>.
   * 
   * @param m               Forwarded to \c amaru_mshift.
   * @param u               Forwarded to \c amaru_mshift.
   * @param l               Forwarded to \c amaru_mshift.
   */
  static amaru_u1_t
  mshift(amaru_u1_t m, amaru_u1_t u, amaru_u1_t l);

};

} // namespace test
} // namespace amaru

#endif // AMARU_CPP_TEST_IMPL_HPP
