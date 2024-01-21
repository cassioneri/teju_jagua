/**
 * @file cpp/generator/generator.hpp
 *
 * Generator of Amaru's implementations.
 */

#ifndef AMARU_CPP_GENERATOR_GENERATOR_HPP_
#define AMARU_CPP_GENERATOR_GENERATOR_HPP_

#include "cpp/generator/config.hpp"
#include "cpp/generator/multiprecision.hpp"

#include <cstdint>
#include <string>

namespace amaru {

/**
 * \brief Generator of Amaru's implementation for a given floating point
 * number type.
 */
struct generator_t {

  /**
   * \brief Constructor.
   *
   * \param info            The information on the floating point number type.
   * \param directory       The directory where generated files are saved.
   */
  generator_t(config_t config, std::string directory);

  /**
   * \brief Generates the implementation.
   */
  void
  generate() const;

private:

  /**
   * \brief Returns the identifier for the floating point number type.
   */
  std::string const&
  id() const;

  /**
   * \brief Returns the size of the limb in bits.
   */
  std::uint32_t
  size() const;

  /**
   * \brief Returns the type prefix corresponding to a given size.
   */
  std::string const&
  prefix() const;

  /**
   * \brief Returns the name of Amaru's conversion function.
   */
  std::string const&
  function() const;

  /**
   * \brief Returns the minimum binary exponent.
   */
  std::int32_t
  exponent_min() const;

  /**
   * \brief Returns maximum binary exponent.
   */
  std::int32_t
  exponent_max() const;

  /**
   * \brief Returns the size of mantissa in bits.
   */
  std::uint32_t
  mantissa_size() const;

  /**
   * \brief Returns the normal (inclusive) minimal mantissa.
   *
   * Mantissas for normal floating point numbers are elements of the interval
   * [normal_mantissa_min(), normal_mantissa_max()[.
   */
  integer_t const&
  mantissa_min() const;

  /**
   * \brief Returns the normal (exclusive) maximal mantissa.
   *
   * Mantissas for normal floating point numbers are elements of the interval
   * [normal_mantissa_min(), normal_mantissa_max()[.
   */
  integer_t const&
  mantissa_max() const;

  /**
   * \brief Returns the number of parts that each stored limb is split into.
   */
  std::uint32_t
  storage_split() const;

  /**
   * \brief Returns the index offset.
   */
  std::int32_t
  index_offset() const;

  /**
   * \brief Returns the calculation method for div10.
   */
  std::string const&
  calculation_div10() const;

  /**
   * \brief Returns the calculation method for mshift.
   */
  std::string const&
  calculation_mshift() const;

  /**
   * \brief Returns the directory where generated files are saved.
   */
  std::string const&
  directory() const;

  /**
   * \brief Returns the name of the generated .h file.
   */
  std::string const&
  dot_h() const;

  /**
   * \brief Returns the name of the generated .c file.
   */
  std::string const&
  dot_c() const;

  /**
   * \brief Streams out the .h file.
   *
   * \param stream          Output stream to receive the content.
   */
  void
  generate_dot_h(std::ostream& stream) const;

  /**
   * \brief Streams out the .c file.
   *
   * \param stream          Output stream to receive the content.
   */
  void
  generate_dot_c(std::ostream& stream) const;

  struct fast_eaf_t;
  struct alpha_delta_maximum_t;

  /**
   * \brief Gets the maxima of all primary problems. (See get_maximum_primary.)
   *
   * It returns a vector v of size exponent_max() - exponent_min() + 1 such that
   * v[i] contains the maximum of the primary problem corresponding to exponent
   * = exponent_min() + i.
   *
   * \returns The vector v.
   */
  std::vector<alpha_delta_maximum_t>
  get_maxima() const;

  /**
   * \brief Given alpha and delta, this function calculates the maximiser of
   * phi(m) over the relevant set of mantissas.
   *
   * \pre 0 <= alpha && 0 < delta.
   */
  rational_t
  get_maximum(integer_t alpha, integer_t const& delta, bool start_at_1) const;

  /**
   * \brief Get the EAF f(m) = alpha * m / delta which works on an interval of
   * relevant mantissas. This fast EAF is associated to maximisation of phi(m)
   * over the set of mantissas.
   *
   * \param x               The container of alpha, beta and the solution of
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

} // namespace amaru

#endif // AMARU_CPP_GENERATOR_GENERATOR_HPP_
