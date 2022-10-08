#pragma once

#include "../generated/ieee32.h"

#ifdef __cplusplus
extern "C" {
#endif

uint32_t amaru_val_to_str_ieee32(float val, char* str);
ieee32_t amaru_val_to_dec_ieee32(float val);

#ifdef __cplusplus
}
#endif
