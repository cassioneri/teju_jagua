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
  uint32_t mantissa;
} ieee32_t;

ieee32_t amaru_bin_to_dec_ieee32(bool negative, int32_t exponent, uint32_t mantissa);

#ifdef __cplusplus
}
#endif
