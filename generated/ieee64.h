// This file was auto-generated. DO NOT EDIT IT.

#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  bool negative;
  int32_t exponent;
  uint64_t mantissa;
} ieee64_t;

ieee64_t amaru_bin_to_dec_ieee64(bool negative, int32_t exponent, uint64_t mantissa);

#ifdef __cplusplus
}
#endif
