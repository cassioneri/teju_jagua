#include "generator/config.hpp"
#include "generator/exception.hpp"

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
  src["limbs"   ].get_to(tgt.limbs   );
  src["exponent"].get_to(tgt.exponent);
}

void
from_json(nlohmann::json const& src, config_t::calculation_t& tgt) {
  src["limbs"].get_to(tgt.limbs);
}

void
from_json(nlohmann::json const& src, config_t& tgt) {
  src["id"         ].get_to(tgt.id         );
  src["size"       ].get_to(tgt.size       );
  src["exponent"   ].get_to(tgt.exponent   );
  src["mantissa"   ].get_to(tgt.mantissa   );
  src["storage"    ].get_to(tgt.storage    );
  src["calculation"].get_to(tgt.calculation);
}

void
validate(config_t const& json) {

  if (!(json.size == 32 || json.size == 64))
    throw exception_t{"Constraint violation: "
      "size in { 32, 64 }."};

  if (!(json.exponent.minimum <= json.exponent.maximum))
    throw exception_t{"Constraint violation: "
      "exponent.minimum <= exponent.maximum."};

  auto const p2size = std::uint64_t(1) << json.size;
  if (!(json.exponent.maximum - json.exponent.minimum < p2size))
    throw exception_t{"Constraint violation: "
      "exponent.maximum - exponent.minimum <= 2^{exponent.size}."};

  if (!(uint64_t(json.exponent.size) + json.mantissa.size <= json.size))
    throw exception_t{"Constraint violation: "
      "exponent.size + mantissa.size <= size."};

  if (!(json.storage.limbs == 1 || json.storage.limbs == 2))
    throw exception_t{"Constraint violation: "
      "storage.limbs in { 1, 2 }."};

  if (!(json.calculation.limbs == 1 || json.calculation.limbs == 2))
    throw exception_t{"Constraint violation: "
      "calculation.limbs in { 1, 2, 4 }."};
}

} // namespace amaru
