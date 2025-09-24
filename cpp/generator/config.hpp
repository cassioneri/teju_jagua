// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

/**
 * @file cpp/generator/config.hpp
 *
 * Configuration of Tejú Jaguá's implementations.
 */

#ifndef TEJU_CPP_GENERATOR_CONFIG_HPP_
#define TEJU_CPP_GENERATOR_CONFIG_HPP_

#include "teju/src/config.h"

#include <nlohmann/json.hpp>

#include <cstdint>
#include <string>

namespace teju {

/**
 * @brief Configuration of Tejú Jaguá's implementation.
 */
struct config_t {

  // An identifier for the floating-point number type (e.g. "ieee32" or
  // "ieee64".) This is used in C/C++ identifiers and, accordingly, the set
  // of accepted characters is defined by the C and C++ standards. In
  // particular, it must not contain spaces -- "long double" is forbidden.
  std::string id;

  // Limb width (number of bits).
  std::uint32_t width;

  struct spdx_t {
    std::string              identifier;
    std::vector<std::string> copyright;
  } spdx;

  struct exponent_t {

    // Minimum binary exponent.
    std::int32_t minimum;

    // Maximum binary exponent.
    std::int32_t maximum;

  } exponent;

  struct mantissa_t {

    // Mantissa width (number of bits).
    std::uint32_t width;

  } mantissa;

  struct storage_t {

    // The upper and lower limb of the multiplier can be further split into
    // 1 (no split), 2 or 4 parts. This is useful for large types (e.g.
    // __uint128_t) for which the platform does not support literals of.
    std::uint32_t split;

    // The endianness of the storage, i.e. if the 2-limb multiplier is to be
    // stored as { lower, upper } (endianness == "little") or { upper, lower}
    // (endianness == "big"). It doesn't have to match the endianness of the
    // target system, but there might be a performance penalty if it does not.
    std::string endianness;

  } storage;

  struct calculation_t {

    // Defines the platform support for multiplication to be used by
    // div10(). It can be "", "built_in_1", "synthetic_1" or "built_in_2".
    std::string div10;

    // Defines the platform support for multiplication to be used by
    // mshift(). It can be "built_in_1", "synthetic_1", "built_in_2",
    // "synthetic_2" or "built_in_4".
    std::string mshift;

  } calculation;
}; // struct config_t

void
from_json(nlohmann::json const& src, config_t::exponent_t& tgt);

void
from_json(nlohmann::json const& src, config_t::mantissa_t& tgt);

void
from_json(nlohmann::json const& src, config_t::storage_t& tgt);

void
from_json(nlohmann::json const& src, config_t::calculation_t& tgt);

void
from_json(nlohmann::json const& src, config_t& tgt);

void
validate(config_t const& config);

} // namespace teju

#endif // TEJU_CPP_GENERATOR_CONFIG_HPP_
