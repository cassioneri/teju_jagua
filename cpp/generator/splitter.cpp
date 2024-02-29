// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2024 Cassio Neri <cassio.neri@gmail.com>

#include "cpp/generator/splitter.hpp"

#include <utility>

namespace teju {

splitter_t::splitter_t(std::uint32_t size, std::uint32_t parts) :
  size_ {std::move(size) },
  parts_{std::move(parts)} {
}

std::uint32_t
splitter_t::size() const {
  return size_;
}

std::uint32_t
splitter_t::parts() const {
  return parts_;
}

splitter_t::data_t
splitter_t::operator()(integer_t value) const {
  return {*this, std::move(value)};
}

splitter_t::data_t::data_t(splitter_t splitter, integer_t value) :
  splitter_{std::move(splitter)},
  value_   {std::move(value)   } {
}

splitter_t const&
splitter_t::data_t::splitter() const {
  return splitter_;
}

integer_t&
splitter_t::data_t::value() {
  return value_;
}

integer_t const&
splitter_t::data_t::value() const {
  return value_;
}

std::ostream&
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
