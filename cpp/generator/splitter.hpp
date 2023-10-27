#ifndef AMARU_CPP_GENERATOR_SPLITTER_HPP_
#define AMARU_CPP_GENERATOR_SPLITTER_HPP_

#include "cpp/generator/multiprecision.hpp"

namespace amaru {

/**
 * \brief Splits a number into smaller pieces.
 *
 * Generated sources might need to hardcode large numbers but the platform might
 * lack support for literals of the required size. This class helps splitting
 * such number into smaller pieces which can be given to macros amaru_pack2 or
 * amaru_pack2 for the large number to be reconstructed.
 *
 * A typical usage looks like this:
 *
 *     splitter_t splitter{128, 2};
 *     integer_t n = (integer_t{0x0123456789abcdef} << 64) + 0xfedcba9876543210;
 *     std::cout << splitter(n) << '\n';
 *
 * Which streams:
 *
 *     amaru_pack2(0x0123456789abcdef, 0xfedcba9876543210)
 */
struct splitter_t {

  /**
   * \brief Constructor.
   *
   * \param size              The limb size to be splitted.
   * \param parts             Number of parts that the limbs must be split into.
   */
  splitter_t(std::uint32_t size, std::uint32_t parts) :
    size {std::move(size) },
    parts{std::move(parts)} {
  }

  struct data_t;

  /**
   * \brief Returns an object which, when streamed out,  splits n.
   */
  data_t
  operator()(integer_t n) const;

  std::uint32_t size;
  std::uint32_t parts;
};

/**
 * \brief Created by splitter_t::operator(), it holds information for splitting
 * a given number.
 */
struct splitter_t::data_t {

  /**
   * \brief Constructor.
   *
   * \param splitter          The splitter that created *this.
   * \param n                 The number to be split.
   */
  data_t(splitter_t splitter, integer_t n) :
    splitter{splitter    },
    n       {std::move(n)} {
  }

  splitter_t splitter;
  integer_t  n;
};

splitter_t::data_t
splitter_t::operator()(integer_t n) const {
  return {*this, std::move(n)};
}

/**
 * \brief The operator for data.
 *
 * At the time of construction, data received a splitter and a number n. This
 * stream operator uses splitter's fields to configure the splitting of n.
 *
 * \param os                  The object to be streamed to.
 * \param data                The data taken by r-value reference.
 */
std::ostream&
operator<<(std::ostream& os, splitter_t::data_t&& data) {

  if (data.splitter.parts == 1)
    return os << "0x" << std::hex << std::setw(data.splitter.size / 4) <<
      std::setfill('0') << data.n;

  auto const sub_size = data.splitter.size / data.splitter.parts;
  auto       k        = data.splitter.parts - 1;
  auto       base     = integer_t{1} << (k * sub_size);
  integer_t u;

  os << "amaru_pack" << data.splitter.parts << '(';

  goto skip_comma;
  while (k) {

    --k;
    base >>= sub_size;

    os << ", ";
    skip_comma:

    divide_qr(data.n, base, u, data.n);

    os << "0x" << std::hex << std::setw(sub_size / 4) <<
      std::setfill('0') << u;
  }

  return os << ')';
}
 
} // namespace amaru

#endif // AMARU_CPP_GENERATOR_SPLITTER_HPP_
