// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2024 Cassio Neri <cassio.neri@gmail.com>

#include "cpp/common/dragonbox.hpp"

namespace teju::dragonbox {

using namespace jkj::dragonbox;
using jkj::dragonbox::to_decimal;

result_t<float>
to_decimal(float const value) {
  return to_decimal(value, policy::sign::ignore, policy::cache::full);
}

result_t<double>
to_decimal(double const value) {
  return to_decimal(value, policy::sign::ignore, policy::cache::full);
}

} // namespace teju::dragonbox
