#pragma once

// This file is auto-generated. DO NOT EDIT IT.

#include <stdint.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  bool    negative;
  int32_t exponent;
  uint64_t mantissa;
} ieee64_t;

ieee64_t amaru_ieee64(ieee64_t binary);

#ifdef __cplusplus
}
#endif
