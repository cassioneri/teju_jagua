// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

// This file was generated. DO NOT EDIT IT.

#include "ieee16_no_uint128.h"

#include "teju/literal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define teju_size                 32u
#define teju_exponent_min         -24
#define teju_mantissa_size        11u
#define teju_storage_index_offset -8
#define teju_calculation_div10    teju_built_in_2
#define teju_calculation_mshift   teju_built_in_2
#define teju_calculation_shift    64u

#define teju_function             teju_ieee16_no_uint128
#define teju_fields_t             teju32_fields_t
#define teju_u1_t                 teju32_u1_t

#if defined(teju32_u2_t)
  #define teju_u2_t               teju32_u2_t
#endif

#if defined(teju32_u4_t)
  #define teju_u4_t               teju32_u4_t
#endif

typedef struct {
  teju_u1_t const upper;
  teju_u1_t const lower;
} teju_multiplier_t;

static const teju_multiplier_t multipliers[] = {
  { 0xbebc2000, 0x00000001 }, // -8
  { 0x98968000, 0x00000001 }, // -7
  { 0xf4240000, 0x00000001 }, // -6
  { 0xc3500000, 0x00000001 }, // -5
  { 0x9c400000, 0x00000001 }, // -4
  { 0xfa000000, 0x00000001 }, // -3
  { 0xc8000000, 0x00000001 }, // -2
  { 0xa0000000, 0x00000001 }, // -1
  { 0x80000000, 0x00000001 }, // 0
  { 0xcccccccc, 0xcccccccd }, // 1
};

#define teju_calculation_sorted 1u

static struct {
  teju_u1_t const multiplier;
  teju_u1_t const bound;
} const minverse[] = {
  { 0x00000001, 0xffffffff }, // 0
  { 0xcccccccd, 0x33333333 }, // 1
  { 0xc28f5c29, 0x0a3d70a3 }, // 2
  { 0x26e978d5, 0x020c49ba }, // 3
  { 0x3afb7e91, 0x0068db8b }, // 4
  { 0x0bcbe61d, 0x0014f8b5 }, // 5
  { 0x68c26139, 0x000431bd }, // 6
  { 0xae8d46a5, 0x0000d6bf }, // 7
  { 0x22e90e21, 0x00002af3 }, // 8
};

#include "teju/teju.h"

#ifdef __cplusplus
}
#endif
