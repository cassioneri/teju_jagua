// This file is auto-generated. DO NOT EDIT IT.

#include "../generated/ieee32.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t suint_t;
typedef uint64_t duint_t;
typedef ieee32_t rep_t;

enum {
  ssize            = 32,
  mantissa_size    = 23,
  bin_exponent_min = -149,
  dec_exponent_min = -45,
  shift            = 29
};

#define AMARU_USE_COMPACT_TBL

static struct {
  suint_t  const upper;
  suint_t  const lower;
} const multipliers[] = {
  { 0x166bb7f0, 0x435c9e72 }, // -45
  { 0x11efc659, 0xcf7d4b8e }, // -44
  { 0x1cb2d6f6, 0x18c878e4 }, // -43
  { 0x16f578c4, 0xe0a060b6 }, // -42
  { 0x125dfa37, 0x1a19e6f8 }, // -41
  { 0x1d6329f1, 0xc35ca4c0 }, // -40
  { 0x178287f4, 0x9c4a1d67 }, // -39
  { 0x12ced32a, 0x16a1b11f }, // -38
  { 0x1e17b843, 0x57691b65 }, // -37
  { 0x1812f9cf, 0x7920e2b7 }, // -36
  { 0x13426172, 0xc74d822c }, // -35
  { 0x1ed09bea, 0xd87c0379 }, // -34
  { 0x18a6e322, 0x46c99c61 }, // -33
  { 0x13b8b5b5, 0x056e16b4 }, // -32
  { 0x1f8def88, 0x08b02453 }, // -31
  { 0x193e5939, 0xa08ce9dc }, // -30
  { 0x1431e0fa, 0xe6d7217d }, // -29
  { 0x1027e72f, 0x1f128131 }, // -28
  { 0x19d971e4, 0xfe8401e8 }, // -27
  { 0x14adf4b7, 0x320334ba }, // -26
  { 0x108b2a2c, 0x28029095 }, // -25
  { 0x1a784379, 0xd99db421 }, // -24
  { 0x152d02c7, 0xe14af681 }, // -23
  { 0x10f0cf06, 0x4dd59201 }, // -22
  { 0x1b1ae4d6, 0xe2ef5001 }, // -21
  { 0x15af1d78, 0xb58c4001 }, // -20
  { 0x1158e460, 0x913d0001 }, // -19
  { 0x1bc16d67, 0x4ec80001 }, // -18
  { 0x16345785, 0xd8a00001 }, // -17
  { 0x11c37937, 0xe0800001 }, // -16
  { 0x1c6bf526, 0x34000001 }, // -15
  { 0x16bcc41e, 0x90000001 }, // -14
  { 0x12309ce5, 0x40000001 }, // -13
  { 0x1d1a94a2, 0x00000001 }, // -12
  { 0x174876e8, 0x00000001 }, // -11
  { 0x12a05f20, 0x00000001 }, // -10
  { 0x1dcd6500, 0x00000001 }, // -9
  { 0x17d78400, 0x00000001 }, // -8
  { 0x1312d000, 0x00000001 }, // -7
  { 0x1e848000, 0x00000001 }, // -6
  { 0x186a0000, 0x00000001 }, // -5
  { 0x13880000, 0x00000001 }, // -4
  { 0x1f400000, 0x00000001 }, // -3
  { 0x19000000, 0x00000001 }, // -2
  { 0x14000000, 0x00000001 }, // -1
  { 0x10000000, 0x00000001 }, // 0
  { 0x19999999, 0x9999999a }, // 1
  { 0x147ae147, 0xae147ae2 }, // 2
  { 0x10624dd2, 0xf1a9fbe8 }, // 3
  { 0x1a36e2eb, 0x1c432ca6 }, // 4
  { 0x14f8b588, 0xe368f085 }, // 5
  { 0x10c6f7a0, 0xb5ed8d37 }, // 6
  { 0x1ad7f29a, 0xbcaf4858 }, // 7
  { 0x15798ee2, 0x308c39e0 }, // 8
  { 0x112e0be8, 0x26d694b3 }, // 9
  { 0x1b7cdfd9, 0xd7bdbab8 }, // 10
  { 0x15fd7fe1, 0x79649560 }, // 11
  { 0x11979981, 0x2dea111a }, // 12
  { 0x1c25c268, 0x497681c3 }, // 13
  { 0x16849b86, 0xa12b9b02 }, // 14
  { 0x1203af9e, 0xe756159c }, // 15
  { 0x1cd2b297, 0xd889bc2c }, // 16
  { 0x170ef546, 0x46d4968a }, // 17
  { 0x12725dd1, 0xd243aba1 }, // 18
  { 0x1d83c94f, 0xb6d2ac35 }, // 19
  { 0x179ca10c, 0x9242235e }, // 20
  { 0x12e3b40a, 0x0e9b4f7e }, // 21
  { 0x1e392010, 0x175ee597 }, // 22
  { 0x182db340, 0x12b25145 }, // 23
  { 0x1357c299, 0xa88ea76b }, // 24
  { 0x1ef2d0f5, 0xda7dd8ab }, // 25
  { 0x18c240c4, 0xaecb13bc }, // 26
  { 0x13ce9a36, 0xf23c0fca }, // 27
  { 0x1fb0f6be, 0x50601942 }, // 28
  { 0x195a5efe, 0xa6b34768 }, // 29
  { 0x14484bfe, 0xebc29f87 }, // 30
  { 0x1039d665, 0x89687f9f }, // 31
};

static struct {
  suint_t const multiplier;
  suint_t const bound;
} const minverse[] = {
  { 0x00000001, 0xffffffff },
  { 0xcccccccd, 0x33333333 },
  { 0xc28f5c29, 0x0a3d70a3 },
  { 0x26e978d5, 0x020c49ba },
  { 0x3afb7e91, 0x0068db8b },
  { 0x0bcbe61d, 0x0014f8b5 },
  { 0x68c26139, 0x000431bd },
  { 0xae8d46a5, 0x0000d6bf },
  { 0x22e90e21, 0x00002af3 },
  { 0x3a2e9c6d, 0x00000897 },
  { 0x3ed61f49, 0x000001b7 },
};

#ifdef __cplusplus
}
#endif

#define AMARU_FUNCTION amaru_bin_to_dec_ieee32
#include "../include/amaru.h"

#undef AMARU_FUNCTION
#undef AMARU_USE_COMPACT_TBL
