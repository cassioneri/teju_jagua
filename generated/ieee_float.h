// This file is auto-generated. DO NOT EDIT IT.

#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  bool     negative;
  int32_t  exponent;
  uint32_t mantissa;
} ieee_float_t;

ieee_float_t amaru_ieee_float(ieee_float_t binary);

#ifdef __cplusplus
}
#endif
