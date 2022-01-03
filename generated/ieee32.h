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
  uint32_t mantissa;
} ieee32_t;

ieee32_t amaru_ieee32(bool negative, int32_t exponent, uint32_t mantissa);

#ifdef __cplusplus
}
#endif
