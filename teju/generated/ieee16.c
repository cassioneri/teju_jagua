// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

// This file was generated. DO NOT EDIT IT.

#include "ieee16.h"

#include "teju/literal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define teju_size                 16u
#define teju_exponent_minimum     -24
#define teju_mantissa_size        10u
#define teju_storage_index_offset -8
#define teju_calculation_sorted   1u
#define teju_calculation_div10    teju_built_in_2
#define teju_calculation_mshift   teju_built_in_4
#define teju_calculation_shift    32u

#define teju_function             teju_ieee16
#define teju_fields_t             teju16_fields_t
#define teju_u1_t                 teju16_u1_t

#if defined(teju16_u2_t)
  #define teju_u2_t               teju16_u2_t
#endif

#if defined(teju16_u4_t)
  #define teju_u4_t               teju16_u4_t
#endif

static struct {
  teju_u1_t const upper;
  teju_u1_t const lower;
} const multipliers[] = {
  { 0xbebc, 0x2001 }, // -8
  { 0x9896, 0x8001 }, // -7
  { 0xf424, 0x0001 }, // -6
  { 0xc350, 0x0001 }, // -5
  { 0x9c40, 0x0001 }, // -4
  { 0xfa00, 0x0001 }, // -3
  { 0xc800, 0x0001 }, // -2
  { 0xa000, 0x0001 }, // -1
  { 0x8000, 0x0001 }, // 0
  { 0xcccc, 0xcccd }, // 1
};

static struct {
  teju_u1_t const multiplier;
  teju_u1_t const bound;
} const minverse[] = {
  { 0x0001, 0xffff },
  { 0xcccd, 0x3333 },
  { 0x5c29, 0x0a3d },
  { 0x78d5, 0x020c },
  { 0x7e91, 0x0068 },
  { 0xe61d, 0x0014 },
  { 0x6139, 0x0004 },
  { 0x46a5, 0x0000 },
  { 0x0e21, 0x0000 },
};

#include "teju/teju.h"

#ifdef __cplusplus
}
#endif
