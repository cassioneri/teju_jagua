#pragma once

// This file is auto-generated. DO NOT EDIT IT.

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#else
#include <stdbool.h>
#endif

typedef struct {
  bool    negative;
  int32_t exponent;
  uint64_t mantissa;
} ieee64_t;

ieee64_t to_amaru_dec_ieee64(bool negative, int32_t exponent, uint64_t mantissa);
#ifdef __cplusplus
}
#endif
