// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

/**
 * @file cpp/generator/splitter.hpp
 *
 * Functionalities to split a large literal into smaller pieces.
 */

#ifndef TEJU_CPP_GENERATOR_SPLITTER_HPP_
#define TEJU_CPP_GENERATOR_SPLITTER_HPP_

#include "cpp/generator/multiprecision.hpp"

#include <cstdint>

namespace teju {

/**
 * @brief Splits a large literal into smaller pieces.
 *
 * Generated sources might need to hardcode large numbers but the platform might
 * lack support for literals of the required size. This class helps splitting
 * such number into smaller pieces which can be given to macros teju_literal2
 * or teju_literal4 for the large number to be reconstructed.
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
 *     teju_literal2(0x0123456789abcdef, 0xfedcba9876543210)
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

inline splitter_t::splitter_t(std::uint32_t size, std::uint32_t parts) :
  size_ {std::move(size) },
  parts_{std::move(parts)} {
}

inline std::uint32_t
splitter_t::size() const {
  return size_;
}

inline std::uint32_t
splitter_t::parts() const {
  return parts_;
}

inline splitter_t::data_t
splitter_t::operator()(integer_t value) const {
  return {*this, std::move(value)};
}

inline splitter_t::data_t::data_t(splitter_t splitter, integer_t value) :
  splitter_{std::move(splitter)},
  value_   {std::move(value)   } {
}

inline splitter_t const&
splitter_t::data_t::splitter() const {
  return splitter_;
}

inline integer_t&
splitter_t::data_t::value() {
  return value_;
}

inline integer_t const&
splitter_t::data_t::value() const {
  return value_;
}

inline std::ostream&
operator<<(std::ostream& os, splitter_t::data_t&& data) {

  auto const size  = data.splitter().size();
  auto const parts = data.splitter().parts();

  if (parts == 1)
    return os << "0x" << std::hex << std::setw(size / 4) << std::setfill('0') <<
      data.value();

  auto const sub_size = size / parts;
  auto       k        = parts - 1;

  // The cast to int32_t is a workaround for a weird gcc bug which appeared
  // after version 11.1: https://godbolt.org/z/91MYsPd1E
  integer_t  base     = integer_t{1} << int32_t(k * sub_size);
  integer_t  u;

  os << "teju_literal" << parts << '(';

  goto skip_comma;
  while (k) {

    --k;
    base >>= sub_size;

    os << ", ";
    skip_comma:

    divide_qr(data.value(), base, u, data.value());

    os << "0x" << std::hex << std::setw(sub_size / 4) <<
      std::setfill('0') << u;
  }

  return os << ')';
}

} // namespace teju

#endif // TEJU_CPP_GENERATOR_SPLITTER_HPP_
