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
} ieee32_t;

ieee32_t amaru_ieee32(ieee32_t binary);

#ifdef __cplusplus
}
#endif
