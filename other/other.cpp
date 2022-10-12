#include "other.hpp"

#include <cmath>

template <typename TResult>
static TResult
zero(bool const is_negative) {
  return { is_negative, 0, 0 };
}

namespace amaru {

namespace dragonbox_compact {

result_float_t
to_decimal(float const value) {
  return value == 0 ? zero<result_float_t>(std::signbit(value)) :
    jkj::dragonbox::to_decimal(value, jkj::dragonbox::policy::cache::compact);
}

result_double_t
to_decimal(double const value) {
  return value == 0 ? zero<result_double_t>(std::signbit(value)) :
    jkj::dragonbox::to_decimal(value, jkj::dragonbox::policy::cache::compact);
}

} // namespace dragonbox_compact

namespace dragonbox_full {

result_float_t
to_decimal(float const value) {
  return value == 0 ? zero<result_float_t>(std::signbit(value)) :
    jkj::dragonbox::to_decimal(value, jkj::dragonbox::policy::cache::full);
}

result_double_t
to_decimal(double const value) {
  return value == 0 ? zero<result_double_t>(std::signbit(value)) :
    jkj::dragonbox::to_decimal(value, jkj::dragonbox::policy::cache::full);
}

} // namespace dragonbox_full

} // namespace amaru
