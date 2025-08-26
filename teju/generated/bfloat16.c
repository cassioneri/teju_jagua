// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

// This file was generated. DO NOT EDIT IT.

#include "bfloat16.h"

#include "teju/literal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define teju_width                16u
#define teju_exponent_min         -133
#define teju_mantissa_width       8u
#define teju_storage_index_offset -41
#define teju_calculation_div10    teju_built_in_2
#define teju_calculation_mshift   teju_built_in_4

#define teju_function             teju_bfloat16
#define teju_fields_t             teju16_fields_t
#define teju_u1_t                 teju16_u1_t

#if defined(teju16_u2_t)
  #define teju_u2_t               teju16_u2_t
#endif

#if defined(teju16_u4_t)
  #define teju_u4_t               teju16_u4_t
#endif

typedef struct {
  teju_u1_t const lower;
  teju_u1_t const upper;
} teju_multiplier_t;

static const teju_multiplier_t multipliers[] = {
  { 0xd1b9, 0x92ef }, // -41
  { 0x4f8f, 0xeb19 }, // -40
  { 0x3fa5, 0xbc14 }, // -39
  { 0x9951, 0x9676 }, // -38
  { 0xc21b, 0xf0bd }, // -37
  { 0xce7c, 0xc097 }, // -36
  { 0x0b97, 0x9a13 }, // -35
  { 0xdf57, 0xf684 }, // -34
  { 0x1913, 0xc537 }, // -33
  { 0xada9, 0x9dc5 }, // -32
  { 0x7c41, 0xfc6f }, // -31
  { 0xc9ce, 0xc9f2 }, // -30
  { 0x07d8, 0xa18f }, // -29
  { 0x3979, 0x813f }, // -28
  { 0x8f28, 0xcecb }, // -27
  { 0xa5ba, 0xa56f }, // -26
  { 0x5162, 0x8459 }, // -25
  { 0x1bcf, 0xd3c2 }, // -24
  { 0x1640, 0xa968 }, // -23
  { 0x7833, 0x8786 }, // -22
  { 0x26b8, 0xd8d7 }, // -21
  { 0xebc6, 0xad78 }, // -20
  { 0x2305, 0x8ac7 }, // -19
  { 0x6b3b, 0xde0b }, // -18
  { 0xbc2f, 0xb1a2 }, // -17
  { 0xc9c0, 0x8e1b }, // -16
  { 0xa932, 0xe35f }, // -15
  { 0x20f5, 0xb5e6 }, // -14
  { 0xe72b, 0x9184 }, // -13
  { 0xa511, 0xe8d4 }, // -12
  { 0xb741, 0xba43 }, // -11
  { 0xf901, 0x9502 }, // -10
  { 0x2801, 0xee6b }, // -9
  { 0x2001, 0xbebc }, // -8
  { 0x8001, 0x9896 }, // -7
  { 0x0001, 0xf424 }, // -6
  { 0x0001, 0xc350 }, // -5
  { 0x0001, 0x9c40 }, // -4
  { 0x0001, 0xfa00 }, // -3
  { 0x0001, 0xc800 }, // -2
  { 0x0001, 0xa000 }, // -1
  { 0x0001, 0x8000 }, // 0
  { 0xcccd, 0xcccc }, // 1
  { 0x0a3e, 0xa3d7 }, // 2
  { 0x6e98, 0x8312 }, // 3
  { 0x1759, 0xd1b7 }, // 4
  { 0xac48, 0xa7c5 }, // 5
  { 0xbd06, 0x8637 }, // 6
  { 0x94d6, 0xd6bf }, // 7
  { 0x7712, 0xabcc }, // 8
  { 0x5f42, 0x8970 }, // 9
  { 0xfecf, 0xdbe6 }, // 10
  { 0xff0c, 0xafeb }, // 11
  { 0xcc0a, 0x8cbc }, // 12
  { 0x1343, 0xe12e }, // 13
  { 0xdc36, 0xb424 }, // 14
  { 0x7cf8, 0x901d }, // 15
  { 0x94bf, 0xe695 }, // 16
  { 0xaa33, 0xb877 }, // 17
  { 0xee8f, 0x9392 }, // 18
  { 0x4a7e, 0xec1e }, // 19
  { 0x0865, 0xbce5 }, // 20
  { 0xa051, 0x971d }, // 21
  { 0x0081, 0xf1c9 }, // 22
  { 0x9a01, 0xc16d }, // 23
  { 0x14ce, 0x9abe }, // 24
  { 0x87af, 0xf796 }, // 25
  { 0x0626, 0xc612 }, // 26
  { 0xd1b8, 0x9e74 }, // 27
  { 0xb5f3, 0xfd87 }, // 28
  { 0xf7f6, 0xcad2 }, // 29
  { 0x5ff8, 0xa242 }, // 30
  { 0xb32d, 0x81ce }, // 31
  { 0x1eae, 0xcfb1 }, // 32
  { 0x4bbe, 0xa627 }, // 33
  { 0x3c98, 0x84ec }, // 34
  { 0x2dc0, 0xd4ad }, // 35
  { 0x249a, 0xaa24 }, // 36
};

#define teju_calculation_sorted 0u

static struct {
  teju_u1_t const multiplier;
  teju_u1_t const bound;
} const minverse[] = {
  { 0x0001, 0xffff }, // 0
  { 0xcccd, 0x3333 }, // 1
  { 0x5c29, 0x0a3d }, // 2
  { 0x78d5, 0x020c }, // 3
  { 0x7e91, 0x0068 }, // 4
  { 0xe61d, 0x0014 }, // 5
  { 0x6139, 0x0004 }, // 6
  { 0x46a5, 0x0000 }, // 7
};

#include "../src/teju.h"

#ifdef __cplusplus
}
#endif
