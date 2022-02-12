#include "../generated/ieee32.h"
#include "../generated/ieee64.h"

#include "dragonbox.h"

#include <cstdint>

extern "C" {

ieee32_t dragonbox_float(float const value) {
  auto const v = jkj::dragonbox::to_decimal(value);
  return { v.is_negative, v.exponent, v.significand };
}

ieee64_t dragonbox_double(double const value) {
  auto const v = jkj::dragonbox::to_decimal(value);
  return { v.is_negative, v.exponent, v.significand };
}

}
