// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

/**
 * @file cpp/common/traits.cpp
 *
 * Traits for floating-point number types.
 */

#include "teju/common.h"
#include "cpp/common/traits.hpp"

#include <cstring>

namespace teju::detail {

  template <typename T>
  T
  to_value(binary_t<T> const binary) {

    using traits_t                = teju::traits_t<T>;
    using u1_t                    = typename traits_t::u1_t;
    auto constexpr exponent_min   = traits_t::exponent_min;
    auto constexpr mantissa_width = traits_t::mantissa_width;

    auto const bits = binary.exponent == exponent_min
        ? u1_t(binary.mantissa)
        : (u1_t(binary.exponent - exponent_min + 2) << (mantissa_width - 1)) +
            teju_lsb(u1_t, u1_t(binary.mantissa), mantissa_width - 1);

    T value;
    std::memcpy(&value, &bits, sizeof(value));

    return value;
  }

  template float  to_value(binary_t<float >);
  template double to_value(binary_t<double>);

} // namespace teju::detail
