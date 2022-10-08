#pragma once

#include "../generated/ieee64.h"

#ifdef __cplusplus
extern "C" {
#endif

uint32_t amaru_val_to_str_ieee64(double val, char* str);
ieee64_t amaru_val_to_dec_ieee64(double val);

#ifdef __cplusplus
}
#endif
