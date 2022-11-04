#ifndef AMARU_GENERATOR_CONFIG_HPP_
#define AMARU_GENERATOR_CONFIG_HPP_

#include "amaru/types.h"

#include <nlohmann/json.hpp>

#include <cstdint>
#include <string>

namespace amaru {

enum class base_t {
  binary  = 0,
  decimal = 1
};

NLOHMANN_JSON_SERIALIZE_ENUM( base_t, {
  {base_t::binary , "binary" },
  {base_t::decimal, "decimal"},
})

enum class multiply_t {
  built_in_1  = amaru_built_in_1,
  syntectic_1 = amaru_syntectic_1,
  built_in_2  = amaru_built_in_2,
  syntectic_2 = amaru_syntectic_2,
  built_in_4  = amaru_built_in_4
};

NLOHMANN_JSON_SERIALIZE_ENUM( multiply_t, {
  {multiply_t::built_in_1 , "built_in_1"  },
  {multiply_t::syntectic_1, "syntectic_1" },
  {multiply_t::built_in_2 , "built_in_2"  },
  {multiply_t::syntectic_2, "syntectic_2" },
  {multiply_t::built_in_4 , "built_in_4"  },
})

/**
 * \brief Configuration of Amaru's implementation.
 */
struct config_t {

  // An identifier for the floating point number type (e.g., "ieee32" or
  // "ieee64".) This is used in C/C++ identifiers and, accordingly, the set
  // of accepted characters is defined by the C and C++ standards. In
  // particular, it must not contain spaces -- "long double" is forbidden.
  std::string id;

  // The size of the limb in bits.
  std::uint32_t size;

  struct exponent_t {

    // Size of exponent in bits.
    std::uint32_t size;

    // Minimum binary exponent.
    std::int32_t minimum;

    // Maximum binary exponent.
    std::int32_t maximum;

  } exponent;

  struct mantissa_t {

    // Size of mantissa in bits.
    std::uint32_t size;

  } mantissa;

  struct storage_t {
    base_t base;
  } storage;

  struct calculation_t {
    multiply_t div10;
    multiply_t infimum;
  } calculation;

  struct optimisation_t {
    bool integer;
    bool mid_point;
  } optimisation;

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

} // namespace amaru

#endif // AMARU_GENERATOR_CONFIG_HPP_
