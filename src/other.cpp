#include "other.hpp"
#include "../tp/dragonbox/include/dragonbox/dragonbox.h"

namespace other {

ieee32_t decimal(float const value) {
  auto const v = jkj::dragonbox::to_decimal(value);
  return { v.is_negative, v.exponent, v.significand };
}

ieee64_t decimal(double const value) {
  auto const v = jkj::dragonbox::to_decimal(value);
  return { v.is_negative, v.exponent, v.significand };
}

} // namespace other
