// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

/**
 * @file cpp/generator/generator.hpp
 *
 * Generator of Teju Jagua's implementations.
 */

#ifndef TEJU_CPP_GENERATOR_GENERATOR_HPP_
#define TEJU_CPP_GENERATOR_GENERATOR_HPP_

#include "cpp/generator/config.hpp"
#include "cpp/generator/multiprecision.hpp"

#include <cstdint>
#include <string>

namespace teju {

/**
 * @brief Generator of Teju Jagua's implementation for a given floating-point
 * number type.
 */
struct generator_t {

  /**
   * @brief Constructor.
   *
   * @param  info           The information on the floating-point number type.
   * @param  directory      The directory where generated files are saved.
   */
  generator_t(config_t config, std::string directory);

  /**
   * @brief Generates the implementation.
   */
  void
  generate() const;

private:

  /**
   * @brief Returns the identifier for the floating-point number type.
   */
  std::string const&
  id() const;

  /**
   * @brief Returns the size of the limb in bits.
   */
  std::uint32_t
  size() const;

  /**
   * @brief Returns the SPDX identifier.
   */
  std::string const&
  spdx_identifier() const;

  /**
   * @brief Returns the SPDX identifier.
   */
  std::vector<std::string> const&
  spdx_copyright() const;

  /**
   * @brief Returns the type prefix corresponding to a given size.
   */
  std::string const&
  prefix() const;

  /**
   * @brief Returns the name of Teju Jagua's conversion function.
   */
  std::string const&
  function() const;

  /**
   * @brief Returns the minimum binary exponent.
   */
  std::int32_t
  exponent_min() const;

  /**
   * @brief Returns maximum binary exponent.
   */
  std::int32_t
  exponent_max() const;

  /**
   * @brief Returns the size of mantissa in bits.
   */
  std::uint32_t
  mantissa_size() const;

  /**
   * @brief Returns the (normal) minimal mantissa.
   */
  integer_t const&
  mantissa_min() const;

  /**
   * @brief Returns the maximal mantissa.
   */
  integer_t const&
  mantissa_max() const;

  /**
   * @brief Returns the number of parts that each stored limb is split into.
   */
  std::uint32_t
  storage_split() const;

  /**
   * @brief Returns the index offset.
   */
  std::int32_t
  index_offset() const;

  /**
   * @brief Returns the calculation method for div10.
   */
  std::string const&
  calculation_div10() const;

  /**
   * @brief Returns the calculation method for mshift.
   */
  std::string const&
  calculation_mshift() const;

  /**
   * @brief Returns the directory where generated files are saved.
   */
  std::string const&
  directory() const;

  /**
   * @brief Returns the name of the generated .h file.
   */
  std::string const&
  dot_h() const;

  /**
   * @brief Returns the name of the generated .c file.
   */
  std::string const&
  dot_c() const;

  /**
   * @brief Streams out the license information.
   *
   * @param  stream         Output stream to receive the content.
   *
   * @returns stream
   */
  std::ostream&
  generate_license(std::ostream& stream) const;

  /**
   * @brief Streams out the .h file.
   *
   * @param  stream         Output stream to receive the content.
   */
  void
  generate_dot_h(std::ostream& stream) const;

  /**
   * @brief Streams out the .c file.
   *
   * @param  stream         Output stream to receive the content.
   */
  void
  generate_dot_c(std::ostream& stream) const;

  struct fast_eaf_t;
  struct alpha_delta_maximum_t;

  /**
   * @brief Gets the maxima of all primary problems. (See get_maximum.)
   *
   * It returns a vector v of size exponent_max() - exponent_min() + 1 such that
   * v[i] contains the maximum of the primary problem corresponding to exponent
   * = exponent_min() + i.
   *
   * @returns The vector v.
   */
  std::vector<alpha_delta_maximum_t>
  get_maxima() const;

  /**
   * @brief Given alpha and delta, this function calculates the maximiser of
   * phi_1(n) over the relevant set of values.
   *
   * @param  alpha          Parameter alpha.
   * @param  delta          Parameter delta.
   * @param  start_at_1     True if the set of relevant mantissas start at 1, or
   *                        false if it starts at mantissa_min().
   *
   * @pre 0 <= alpha && 0 < delta.
   *
   * @returns The maximiser of phi(m) over the relevant set of mantissas.
   */
  rational_t
  get_maximum(integer_t alpha, integer_t const& delta, bool start_at_1) const;

  /**
   * @brief Get the EAF f(m) = alpha * m / delta which works on an interval of
   *        relevant mantissas. This fast EAF is associated to maximisation of
   *        phi_1(n) over the set of mantissas.
   *
   * @param  x              The container of alpha, beta and the solution of
   *                        the primary maximisation problem.
   */
  fast_eaf_t
  get_fast_eaf(alpha_delta_maximum_t const& x) const;

  config_t     config_;
  std::string  prefix_;
  std::string  function_;
  integer_t    mantissa_min_;
  integer_t    mantissa_max_;
  std::int32_t index_offset_;
  std::string  directory_;
  std::string  dot_h_;
  std::string  dot_c_;

}; // struct generator_t

} // namespace teju

#endif // TEJU_CPP_GENERATOR_GENERATOR_HPP_
