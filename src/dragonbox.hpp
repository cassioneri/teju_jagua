#pragma once

#include "../generated/ieee32.h"
#include "../generated/ieee64.h"

extern "C" {

ieee32_t dragonbox_float(float const value);

ieee64_t dragonbox_double(double const value);

}
