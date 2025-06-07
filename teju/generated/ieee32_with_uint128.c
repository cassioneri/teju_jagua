// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

// This file was generated. DO NOT EDIT IT.

#include "ieee32_with_uint128.h"

#include "teju/literal.h"

#ifdef __cplusplus
extern "C" {
#endif

#define teju_size                 32u
#define teju_exponent_min         -149
#define teju_mantissa_size        24u
#define teju_storage_index_offset -45
#define teju_calculation_div10    teju_built_in_2
#define teju_calculation_mshift   teju_built_in_4
#define teju_calculation_shift    64u

#define teju_function             teju_ieee32_with_uint128
#define teju_fields_t             teju32_fields_t
#define teju_u1_t                 teju32_u1_t

#if defined(teju32_u2_t)
  #define teju_u2_t               teju32_u2_t
#endif

#if defined(teju32_u4_t)
  #define teju_u4_t               teju32_u4_t
#endif

typedef struct {
  teju_u1_t const lower;
  teju_u1_t const upper;
} teju_multiplier_t;

static const teju_multiplier_t multipliers[] = {
  { 0x1ae4f38c, 0xb35dbf82 }, // -45
  { 0x7bea5c70, 0x8f7e32ce }, // -44
  { 0xc643c71a, 0xe596b7b0 }, // -43
  { 0x050305ae, 0xb7abc627 }, // -42
  { 0xd0cf37bf, 0x92efd1b8 }, // -41
  { 0x1ae525fe, 0xeb194f8e }, // -40
  { 0xe250eb32, 0xbc143fa4 }, // -39
  { 0xb50d88f5, 0x96769950 }, // -38
  { 0xbb48db21, 0xf0bdc21a }, // -37
  { 0xc90715b4, 0xc097ce7b }, // -36
  { 0x3a6c115d, 0x9a130b96 }, // -35
  { 0xc3e01bc7, 0xf684df56 }, // -34
  { 0x364ce306, 0xc5371912 }, // -33
  { 0x2b70b59e, 0x9dc5ada8 }, // -32
  { 0x45812297, 0xfc6f7c40 }, // -31
  { 0x04674edf, 0xc9f2c9cd }, // -30
  { 0x36b90be6, 0xa18f07d7 }, // -29
  { 0xf8940985, 0x813f3978 }, // -28
  { 0xf4200f3b, 0xcecb8f27 }, // -27
  { 0x9019a5c9, 0xa56fa5b9 }, // -26
  { 0x401484a1, 0x84595161 }, // -25
  { 0xcceda101, 0xd3c21bce }, // -24
  { 0x0a57b401, 0xa968163f }, // -23
  { 0x6eac9001, 0x87867832 }, // -22
  { 0x177a8001, 0xd8d726b7 }, // -21
  { 0xac620001, 0xad78ebc5 }, // -20
  { 0x89e80001, 0x8ac72304 }, // -19
  { 0x76400001, 0xde0b6b3a }, // -18
  { 0xc5000001, 0xb1a2bc2e }, // -17
  { 0x04000001, 0x8e1bc9bf }, // -16
  { 0xa0000001, 0xe35fa931 }, // -15
  { 0x80000001, 0xb5e620f4 }, // -14
  { 0x00000001, 0x9184e72a }, // -13
  { 0x00000001, 0xe8d4a510 }, // -12
  { 0x00000001, 0xba43b740 }, // -11
  { 0x00000001, 0x9502f900 }, // -10
  { 0x00000001, 0xee6b2800 }, // -9
  { 0x00000001, 0xbebc2000 }, // -8
  { 0x00000001, 0x98968000 }, // -7
  { 0x00000001, 0xf4240000 }, // -6
  { 0x00000001, 0xc3500000 }, // -5
  { 0x00000001, 0x9c400000 }, // -4
  { 0x00000001, 0xfa000000 }, // -3
  { 0x00000001, 0xc8000000 }, // -2
  { 0x00000001, 0xa0000000 }, // -1
  { 0x00000001, 0x80000000 }, // 0
  { 0xcccccccd, 0xcccccccc }, // 1
  { 0x70a3d70b, 0xa3d70a3d }, // 2
  { 0x8d4fdf3c, 0x83126e97 }, // 3
  { 0xe219652c, 0xd1b71758 }, // 4
  { 0x1b478424, 0xa7c5ac47 }, // 5
  { 0xaf6c69b6, 0x8637bd05 }, // 6
  { 0xe57a42bd, 0xd6bf94d5 }, // 7
  { 0x8461cefd, 0xabcc7711 }, // 8
  { 0x36b4a598, 0x89705f41 }, // 9
  { 0xbdedd5bf, 0xdbe6fece }, // 10
  { 0xcb24aaff, 0xafebff0b }, // 11
  { 0x6f5088cc, 0x8cbccc09 }, // 12
  { 0x4bb40e14, 0xe12e1342 }, // 13
  { 0x095cd810, 0xb424dc35 }, // 14
  { 0x3ab0acda, 0x901d7cf7 }, // 15
  { 0xc44de15c, 0xe69594be }, // 16
  { 0x36a4b44a, 0xb877aa32 }, // 17
  { 0x921d5d08, 0x9392ee8e }, // 18
  { 0xb69561a6, 0xec1e4a7d }, // 19
  { 0x92111aeb, 0xbce50864 }, // 20
  { 0x74da7bef, 0x971da050 }, // 21
  { 0xbaf72cb2, 0xf1c90080 }, // 22
  { 0x95928a28, 0xc16d9a00 }, // 23
  { 0x44753b53, 0x9abe14cd }, // 24
  { 0xd3eec552, 0xf79687ae }, // 25
  { 0x76589ddb, 0xc6120625 }, // 26
  { 0x91e07e49, 0x9e74d1b7 }, // 27
  { 0x8300ca0e, 0xfd87b5f2 }, // 28
  { 0x359a3b3f, 0xcad2f7f5 }, // 29
  { 0x5e14fc32, 0xa2425ff7 }, // 30
  { 0x4b43fcf5, 0x81ceb32c }, // 31
};

#define teju_calculation_sorted 0u

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
  { 0x3a2e9c6d, 0x00000897 }, // 9
  { 0x3ed61f49, 0x000001b7 }, // 10
  { 0x0c913975, 0x00000057 }, // 11
  { 0xcf503eb1, 0x00000011 }, // 12
  { 0xf6433fbd, 0x00000003 }, // 13
};

#include "teju/teju.h"

#ifdef __cplusplus
}
#endif
