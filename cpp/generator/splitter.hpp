/**
 * @file cpp/generator/splitter.hpp
 *
 * Functionalities to split a large literal into smaller pieces.
 */

#ifndef AMARU_CPP_GENERATOR_SPLITTER_HPP_
#define AMARU_CPP_GENERATOR_SPLITTER_HPP_

#include "cpp/generator/multiprecision.hpp"

#include <cstdint>

namespace amaru {

/**
 * @brief Splits a large literal into smaller pieces.
 *
 * Generated sources might need to hardcode large numbers but the platform might
 * lack support for literals of the required size. This class helps splitting
 * such number into smaller pieces which can be given to macros amaru_literal2
 * or amaru_literal4 for the large number to be reconstructed.
 *
 * A typical usage looks like the following. Consider a 128-bits unsigned n
 * whose value is 0x0123456789abcdeffedcba9876543210. It can be split into two
 * parts:
 *
 *     splitter_t splitter{128, 2};
 *     std::cout << splitter(n) << '\n';
 *
 * Which streams:
 *
 *     amaru_literal2(0x0123456789abcdef, 0xfedcba9876543210)
 */
struct splitter_t {

  /**
   * @brief Constructor.
   *
   * @param size              The limb size to be split.
   * @param parts             Number of parts that the limbs must be split into.
   */
  splitter_t(std::uint32_t size, std::uint32_t parts);

  struct data_t;

  /**
   * @brief Returns an object which, when streamed out, splits value.
   */
  data_t
  operator()(integer_t value) const;

  /**
   * @brief Gets the size.
   */
  std::uint32_t
  size() const;

  /**
   * @brief Gets the number of parts.
   */
  std::uint32_t
  parts() const;

private:

  std::uint32_t size_;
  std::uint32_t parts_;
};

/**
 * @brief Created by splitter_t::operator(), it holds information for splitting
 * a given number.
 */
struct splitter_t::data_t {

  /**
   * @brief Constructor.
   *
   * @param splitter          The splitter that created *this.
   * @param value             The value to be split.
   */
  data_t(splitter_t splitter, integer_t value);

  /**
   * @brief Gets the splitter.
   */
  splitter_t const&
  splitter() const;

  /**
   * @brief Gets the value.
   */
  integer_t&
  value();

  /**
   * @brief Gets the value.
   */
  integer_t const&
  value() const;

private:

  splitter_t splitter_;
  integer_t  value_;
};

/**
 * @brief The ostream operator for splitter_t::data_t.
 *
 * At the time of construction, data received a splitter and a number n. This
 * stream operator uses splitter's fields to configure the splitting of n.
 *
 * @param os                  The object to be streamed to.
 * @param data                The splitter_t::data_t object.
 */
std::ostream&
operator<<(std::ostream& os, splitter_t::data_t&& data);

} // namespace amaru

#endif // AMARU_CPP_GENERATOR_SPLITTER_HPP_
