// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2024 Cassio Neri <cassio.neri@gmail.com>

/**
 * @file cpp/common/dragonbox.hpp
 *
 * Teju Jagua's wrapper around dragonbox used for testing and benchmarking.
 */

#ifndef TEJU_CPP_COMMON_DRAGONBOX_HPP_
#define TEJU_CPP_COMMON_DRAGONBOX_HPP_

#include <dragonbox/dragonbox.h>

namespace teju::dragonbox {

template <typename T>
using result_t = decltype(jkj::dragonbox::to_decimal(T{0},
  jkj::dragonbox::policy::sign::ignore,
  jkj::dragonbox::policy::cache::full));

result_t<float>
to_decimal(float value);

result_t<double>
to_decimal(double value);

} // namespace teju::dragonbox

#endif // TEJU_CPP_COMMON_DRAGONBOX_HPP_
