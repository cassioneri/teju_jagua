// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2024 Cassio Neri <cassio.neri@gmail.com>

#include "amaru/common.h"
#include "cpp/common/exception.hpp"
#include "cpp/generator/config.hpp"

#include <algorithm>
#include <iterator>
#include <iostream>

namespace amaru {

void
from_json(nlohmann::json const& src, config_t::spdx_t& tgt) {
  src.at("identifier").get_to(tgt.identifier);
  src.at("copyright" ).get_to(tgt.copyright );
}

void
from_json(nlohmann::json const& src, config_t::exponent_t& tgt) {
  src.at("size"   ).get_to(tgt.size   );
  src.at("minimum").get_to(tgt.minimum);
  src.at("maximum").get_to(tgt.maximum);
}

void
from_json(nlohmann::json const& src, config_t::mantissa_t& tgt) {
  src.at("size").get_to(tgt.size);
}

void
from_json(nlohmann::json const& src, config_t::storage_t& tgt) {
  src.at("split").get_to(tgt.split);
}

void
from_json(nlohmann::json const& src, config_t::calculation_t& tgt) {
  if (src.contains("div10"))
    src["div10"].get_to(tgt.div10 );
  src.at("mshift").get_to(tgt.mshift);
}

void
from_json(nlohmann::json const& src, config_t& tgt) {
  src.at("id"          ).get_to(tgt.id          );
  src.at("size"        ).get_to(tgt.size        );
  src.at("spdx"        ).get_to(tgt.spdx        );
  src.at("exponent"    ).get_to(tgt.exponent    );
  src.at("mantissa"    ).get_to(tgt.mantissa    );
  src.at("storage"     ).get_to(tgt.storage     );
  src.at("calculation" ).get_to(tgt.calculation );
}

void
validate(config_t const& json) {

  require(json.size == 32 || json.size == 64 || json.size == 128,
    "Constraint violation: size in { 32, 64 }");

  require(json.exponent.minimum <= json.exponent.maximum,
    "Constraint violation: exponent.minimum <= exponent.maximum");

  std::int32_t min = std::min(amaru_log10_pow2_min, amaru_log10_075_pow2_min);

  require(json.exponent.minimum >= min,
    "Constraint violation: json.exponent.minimum >= min");

  std::int32_t max = std::max(amaru_log10_pow2_max, amaru_log10_075_pow2_max);

  require(json.exponent.maximum <= max,
    "Constraint violation: json.exponent.maximum <= max");

  auto const p2size = std::uint64_t{1} << json.exponent.size;
  require(json.exponent.maximum - json.exponent.minimum < p2size,
    "Constraint violation: "
    "exponent.maximum - exponent.minimum <= 2^{exponent.size}");

  require(std::uint64_t{json.exponent.size} + json.mantissa.size <= json.size,
    "Constraint violation: exponent.size + mantissa.size <= size");

  require(json.storage.split == 1 || json.storage.split == 2 ||
    json.storage.split == 4,
    "Constraint violation: storage.split in { 1, 2, 4 }");

  std::string const multiply_types[] = {
    "", "built_in_1", "synthetic_1", "built_in_2", "synthetic_2",
    "built_in_4"
  };

  auto const i_div10 = std::distance(std::cbegin(multiply_types),
    std::find(std::cbegin(multiply_types), std::cend(multiply_types),
    json.calculation.div10));

  require(i_div10 <= 3,
    "Constraint violation: calculation.div10 in { "
    "\"\", \"built_in_1\", \"synthetic_1\", \"built_in_2\" }");

  auto const i_mshift = std::distance(std::cbegin(multiply_types),
    std::find(std::cbegin(multiply_types), std::cend(multiply_types),
    json.calculation.div10));

  require(i_mshift <= 4,
    "Constraint violation: calculation.mshift in { \"built_in_1\", "
    "\"synthetic_1\", \"built_in_2\", \"synthetic_2\", \"built_in_4\" }");
}

} // namespace amaru
