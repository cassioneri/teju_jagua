#include "generator/config.hpp"
#include "generator/exception.hpp"

#include <iostream>

namespace amaru {

void
from_json(nlohmann::json const& src, config_t::exponent_t& tgt) {
  src["size"   ].get_to(tgt.size   );
  src["minimum"].get_to(tgt.minimum);
  src["maximum"].get_to(tgt.maximum);
}

void
from_json(nlohmann::json const& src, config_t::mantissa_t& tgt) {
  src["size"].get_to(tgt.size);
}

void
from_json(nlohmann::json const& src, config_t::storage_t& tgt) {
  src["base" ].get_to(tgt.base );
}

void
from_json(nlohmann::json const& src, config_t::calculation_t& tgt) {
  src["div10"  ].get_to(tgt.div10  );
  src["infimum"].get_to(tgt.infimum);
}

void
from_json(nlohmann::json const& src, config_t::optimisation_t& tgt) {
  src["integer"  ].get_to(tgt.integer  );
  src["mid_point"].get_to(tgt.mid_point);
}

void
from_json(nlohmann::json const& src, config_t& tgt) {
  src["id"          ].get_to(tgt.id          );
  src["size"        ].get_to(tgt.size        );
  src["exponent"    ].get_to(tgt.exponent    );
  src["mantissa"    ].get_to(tgt.mantissa    );
  src["storage"     ].get_to(tgt.storage     );
  src["calculation" ].get_to(tgt.calculation );
  src["optimisation"].get_to(tgt.optimisation);
}

void
validate(config_t const& json) {

  if (!(json.size == 32 || json.size == 64))
    throw exception_t{"Constraint violation: "
      "size in { 32, 64 }"};

  if (!(json.exponent.minimum <= json.exponent.maximum))
    throw exception_t{"Constraint violation: "
      "exponent.minimum <= exponent.maximum"};

  auto const p2size = std::uint64_t(1) << json.exponent.size;
  if (!(json.exponent.maximum - json.exponent.minimum < p2size))
    throw exception_t{"Constraint violation: "
      "exponent.maximum - exponent.minimum <= 2^{exponent.size}"};

  if (!(uint64_t(json.exponent.size) + json.mantissa.size <= json.size))
    throw exception_t{"Constraint violation: "
      "exponent.size + mantissa.size <= size"};

  if (json.storage.base == base_t::invalid)
    throw exception_t{"Constraint violation: "
      "storage.base in { \"binary\", \"decimal\" } "};

  if (json.calculation.div10 == multiply_t::invalid)
    throw exception_t{"Constraint violation: "
      "calculation.div10 in { \"built_in_1\", \"syntectic_1\", "
      "\"built_in_2\" } "};

  if (json.calculation.infimum == multiply_t::invalid)
    throw exception_t{"Constraint violation: "
      "calculation.infimum in { \"built_in_1\", \"syntectic_1\", "
      "\"built_in_2\", \"syntectic_2\", \"built_in_4\" } "};
}

} // namespace amaru
