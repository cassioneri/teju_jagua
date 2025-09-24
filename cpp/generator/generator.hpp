// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

/**
 * @file cpp/generator/generator.hpp
 *
 * Generator of Tejú Jaguá's implementations.
 */

#ifndef TEJU_CPP_GENERATOR_GENERATOR_HPP_
#define TEJU_CPP_GENERATOR_GENERATOR_HPP_

#include "config.hpp"
#include "multiprecision.hpp"

#include <cstdint>
#include <string>

namespace teju {

/**
 * @brief Generator of Tejú Jaguá's implementation for a given floating-point
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
  [[nodiscard]] std::string const&
  id() const;

  /**
   * @brief Returns the limb width (number of bits).
   */
  [[nodiscard]] std::uint32_t
  width() const;

  /**
   * @brief Returns the SPDX identifier.
   */
  [[nodiscard]] std::string const&
  spdx_identifier() const;

  /**
   * @brief Returns the SPDX identifier.
   */
  [[nodiscard]] std::vector<std::string> const&
  spdx_copyright() const;

  /**
   * @brief Returns the type prefix corresponding to a given width.
   */
  [[nodiscard]] std::string const&
  prefix() const;

  /**
   * @brief Returns the name of Tejú Jaguá's conversion function.
   */
  [[nodiscard]] std::string const&
  function() const;

  /**
   * @brief Returns the minimum binary exponent.
   */
  [[nodiscard]] std::int32_t
  exponent_min() const;

  /**
   * @brief Returns maximum binary exponent.
   */
  [[nodiscard]] std::int32_t
  exponent_max() const;

  /**
   * @brief Returns the mantissa width (number of bits).
   */
  [[nodiscard]] std::uint32_t
  mantissa_width() const;

  /**
   * @brief Returns the (normal) minimal mantissa.
   */
  [[nodiscard]] integer_t const&
  mantissa_min() const;

  /**
   * @brief Returns the maximal mantissa.
   */
  [[nodiscard]] integer_t const&
  mantissa_max() const;

  /**
   * @brief Returns the number of parts that each stored limb is split into.
   */
  [[nodiscard]] std::uint32_t
  storage_split() const;

  /**
   * @brief Returns the endianess of the storage.
   */
  [[nodiscard]] std::string const&
  storage_endianness() const;

  /**
   * @brief Returns the index offset.
   */
  [[nodiscard]] std::int32_t
  index_offset() const;

  /**
   * @brief Returns the calculation method for div10.
   */
  [[nodiscard]] std::string const&
  calculation_div10() const;

  /**
   * @brief Returns the calculation method for mshift.
   */
  [[nodiscard]] std::string const&
  calculation_mshift() const;

  /**
   * @brief Returns the directory where generated files are saved.
   */
  [[nodiscard]] std::string const&
  directory() const;

  /**
   * @brief Returns the name of the generated .h file.
   */
  [[nodiscard]] std::string const&
  dot_h() const;

  /**
   * @brief Returns the name of the generated .c file.
   */
  [[nodiscard]] std::string const&
  dot_c() const;

  /**
   * @brief Streams out the license information.
   *
   * @param  stream         Output stream to receive the content.
   *
   * @returns stream
   */
  [[nodiscard]] std::ostream&
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

  /**
   * @brief Check whether Tejú Jaguá can use the algorithm of Theorem 4 of [1]
   *        to perform division by 10.
   * [1] Neri C, Schneider L. "Euclidean affine functions and their application
   * to calendar algorithms." Softw Pract Exper. 2023; 53(4):937-970.
   * https://onlinelibrary.wiley.com/doi/full/10.1002/spe.3172
   *
   * @returns true if the algorithm can be used or false, otherwise.
   */
  [[nodiscard]] bool
  check_div10_algorithm() const;

  /**
   * @brief Checks whether the calculations done for the centred cases are safe.
   *
   * @returns true if the calculations are safe or false, otherwise.
   */
  [[nodiscard]] bool
  check_centred_calculations() const;

  /**
   * @brief Checks whether the calculations done for the uncentred cases are
   *        safe.
   *
   * @returns true if the calculations are safe or false, otherwise.
   */
  [[nodiscard]] bool
  check_uncentred_calculations() const;

  /**
   * @brief Checks whether the calculations done for the uncentred refined cases
   *        are safe.
   *
   * @returns true if the calculations are safe or false, otherwise.
   */
  [[nodiscard]] bool
  check_uncentred_refined_calculations() const;

  /**
   * @brief Gets the numerator of the fast EAF for n * 2^(e0 - 1) / 10^f which
   *        works on a set of relevant values of n.
   *
   * The binary exponent e_0 must be such that 10^(e_0 - 1) < 10^f <= 10^e_0 for
   * some integer f. (This f is unique and it's the exponent of the EAF's
   * denominator.)
   *
   * This function returns an integer U such that
   *   U * n / 2^k == n * 2^(e0 - 1) / 10^f,
   * where k = 2 * width().
   *
   * @param  e_0            The binary exponent.
   * @param  is_min         Tells whether the exponent e_0 is the minimal one.
   *
   * @returns The numerator of the fast EAF.
   */
  [[nodiscard]] integer_t
  get_fast_eaf_numerator(int32_t e_0, bool is_min) const;

  /**
   * @brief Given alpha and delta, this function calculates the maximum of
   *        phi_1(n) over the relevant set of values.
   *
   * Recall that for each possible exponent yielded by Tejú Jaguá, it defines a
   * maximisation problem whose objective function, phi_1(n), is parameterised
   * on alpha and delta and where n is in a set of values related to mantissas.
   *
   * @param  alpha          Parameter alpha.
   * @param  delta          Parameter delta.
   * @param  is_min         Tells whether the exponent is the minimal one.
   *
   * @pre 0 <= alpha && 0 < delta.
   *
   * @returns The maximum of phi_1(n) over the relevant set of values.
   */
  [[nodiscard]] rational_t
  get_maximum(integer_t alpha, integer_t const& delta, bool is_min) const;

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
