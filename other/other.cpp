#include "other.hpp"

#include <cmath>

namespace amaru {

namespace dragonbox_compact {

result_float_t
to_decimal(float const value) {
  return jkj::dragonbox::to_decimal(value,
    jkj::dragonbox::policy::cache::compact);
}

result_double_t
to_decimal(double const value) {
  return jkj::dragonbox::to_decimal(value,
    jkj::dragonbox::policy::cache::compact);
}

} // namespace dragonbox_compact

namespace dragonbox_full {

result_float_t
to_decimal(float const value) {
  return jkj::dragonbox::to_decimal(value,
    jkj::dragonbox::policy::cache::full);
}

result_double_t
to_decimal(double const value) {
  return jkj::dragonbox::to_decimal(value,
    jkj::dragonbox::policy::cache::full);
}

} // namespace dragonbox_full

} // namespace amaru
