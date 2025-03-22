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
 *        number type.
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

  struct alpha_delta_maximum_t;

  /**
   * @brief Gets the maxima of all primary maximisation problems.
   *
   * Recall that for each possible exponent yielded by Teju Jagua, it defines a
   * maximisation problem whose objective function is parameterised on alpha and
   * delta. (See get_maximum.)
   *
   * This function returns a vector v where each element contains the parameters
   * alpha and delta, and the maximum of the objective function corresponding to
   * an exponent. Elements are ordered by exponent and, in particular, v[0] is
   * the entry for the minimal exponent.
   *
   * @returns The vector v.
   */
  std::vector<alpha_delta_maximum_t>
  get_maxima() const;

  /**
   * @brief Given alpha and delta, this function calculates the maximum of
   *        phi_1(n) over the relevant set of values.
   *
   * Recall that for each possible exponent yielded by Teju Jagua, it defines a
   * maximisation problem whose objective function, phi_1(n), is parameterised
   * on alpha and delta and where n is in a set of values related to mantissas.
   *
   * @param  alpha          Parameter alpha.
   * @param  delta          Parameter delta.
   * @param  is_min         True if the exponent is the minimal one.
   *
   * @pre 0 <= alpha && 0 < delta.
   *
   * @returns The maximum of phi_1(n) over the relevant set of values.
   */
  rational_t
  get_maximum(integer_t alpha, integer_t const& delta, bool is_min) const;

  struct fast_eaf_t;

  /**
   * @brief Gets all fast EAFs.
   *
   * Recall that for each possible exponent yielded by Teju Jagua, it defines a
   * fast EAF used to calculate mantissas. (See get_fast_eaf.)
   *
   * This function returns a vector v where each element contains the fast EAF
   * corresponding to an exponent. Elements are ordered by exponent and, in
   * particular, v[0] is the entry for the minimal exponent.
   *
   * @returns The vector v.
   */
  std::vector<fast_eaf_t>
  get_fast_eafs() const;

  /**
   * @brief Gets the fast EAF for f(n) = alpha * n / delta which works on a set
   *        of relevant values of n.
   *
   * Recall that for each possible exponent yielded by Teju Jagua, it defines a
   * fast EAF used to calculate mantissas and a maximisation problem whose
   * objective function is parameterised on alpha and delta.
   *
   * @param  x              The container of alpha, delta and the maximum of the
   *                        objective function.
   *
   * @returns The fast EAF.
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
