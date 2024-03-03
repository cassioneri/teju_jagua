// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2024 Cassio Neri <cassio.neri@gmail.com>

#include "other.hpp"

#include <cmath>

namespace teju {

namespace dragonbox {

result_float_t
to_decimal(float const value) {
  return jkj::dragonbox::to_decimal(value,
    jkj::dragonbox::policy::sign::ignore,
    jkj::dragonbox::policy::cache::full);
}

result_double_t
to_decimal(double const value) {
  return jkj::dragonbox::to_decimal(value,
    jkj::dragonbox::policy::sign::ignore,
    jkj::dragonbox::policy::cache::full);
}

} // namespace dragonbox

} // namespace teju
