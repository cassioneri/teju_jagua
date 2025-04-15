// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

// This file was generated. DO NOT EDIT IT.

#include "bfloat16.h"

#include "teju/literal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define teju_size                 16u
#define teju_exponent_min         -133
#define teju_mantissa_size        8u
#define teju_storage_index_offset -41
#define teju_calculation_div10    teju_built_in_2
#define teju_calculation_mshift   teju_built_in_4
#define teju_calculation_shift    32u

#define teju_function             teju_bfloat16
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
  { 0x92ef, 0xd1b9 }, // -41
  { 0xeb19, 0x4f8f }, // -40
  { 0xbc14, 0x3fa5 }, // -39
  { 0x9676, 0x9951 }, // -38
  { 0xf0bd, 0xc21b }, // -37
  { 0xc097, 0xce7c }, // -36
  { 0x9a13, 0x0b97 }, // -35
  { 0xf684, 0xdf57 }, // -34
  { 0xc537, 0x1913 }, // -33
  { 0x9dc5, 0xada9 }, // -32
  { 0xfc6f, 0x7c41 }, // -31
  { 0xc9f2, 0xc9ce }, // -30
  { 0xa18f, 0x07d8 }, // -29
  { 0x813f, 0x3979 }, // -28
  { 0xcecb, 0x8f28 }, // -27
  { 0xa56f, 0xa5ba }, // -26
  { 0x8459, 0x5162 }, // -25
  { 0xd3c2, 0x1bcf }, // -24
  { 0xa968, 0x1640 }, // -23
  { 0x8786, 0x7833 }, // -22
  { 0xd8d7, 0x26b8 }, // -21
  { 0xad78, 0xebc6 }, // -20
  { 0x8ac7, 0x2305 }, // -19
  { 0xde0b, 0x6b3b }, // -18
  { 0xb1a2, 0xbc2f }, // -17
  { 0x8e1b, 0xc9c0 }, // -16
  { 0xe35f, 0xa932 }, // -15
  { 0xb5e6, 0x20f5 }, // -14
  { 0x9184, 0xe72b }, // -13
  { 0xe8d4, 0xa511 }, // -12
  { 0xba43, 0xb741 }, // -11
  { 0x9502, 0xf901 }, // -10
  { 0xee6b, 0x2801 }, // -9
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
  { 0xa3d7, 0x0a3e }, // 2
  { 0x8312, 0x6e98 }, // 3
  { 0xd1b7, 0x1759 }, // 4
  { 0xa7c5, 0xac48 }, // 5
  { 0x8637, 0xbd06 }, // 6
  { 0xd6bf, 0x94d6 }, // 7
  { 0xabcc, 0x7712 }, // 8
  { 0x8970, 0x5f42 }, // 9
  { 0xdbe6, 0xfecf }, // 10
  { 0xafeb, 0xff0c }, // 11
  { 0x8cbc, 0xcc0a }, // 12
  { 0xe12e, 0x1343 }, // 13
  { 0xb424, 0xdc36 }, // 14
  { 0x901d, 0x7cf8 }, // 15
  { 0xe695, 0x94bf }, // 16
  { 0xb877, 0xaa33 }, // 17
  { 0x9392, 0xee8f }, // 18
  { 0xec1e, 0x4a7e }, // 19
  { 0xbce5, 0x0865 }, // 20
  { 0x971d, 0xa051 }, // 21
  { 0xf1c9, 0x0081 }, // 22
  { 0xc16d, 0x9a01 }, // 23
  { 0x9abe, 0x14ce }, // 24
  { 0xf796, 0x87af }, // 25
  { 0xc612, 0x0626 }, // 26
  { 0x9e74, 0xd1b8 }, // 27
  { 0xfd87, 0xb5f3 }, // 28
  { 0xcad2, 0xf7f6 }, // 29
  { 0xa242, 0x5ff8 }, // 30
  { 0x81ce, 0xb32d }, // 31
  { 0xcfb1, 0x1eae }, // 32
  { 0xa627, 0x4bbe }, // 33
  { 0x84ec, 0x3c98 }, // 34
  { 0xd4ad, 0x2dc0 }, // 35
  { 0xaa24, 0x249a }, // 36
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

#include "teju/teju.h"

#ifdef __cplusplus
}
#endif
