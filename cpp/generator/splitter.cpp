#include "cpp/generator/splitter.hpp"

#include <utility>

namespace amaru {

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

} // namespace amaru
