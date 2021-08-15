#include <cstdint>
#include <iostream>

using s32  = int32_t;
using u32  = uint32_t;
using u64  = uint64_t;
using u128 = __uint128_t;

// float
constexpr auto P   =   23;
constexpr auto E0  = -149;
constexpr auto L   =    8;

// double
// constexpr auto P   =    52;
// constexpr auto E0  = -1074;
// constexpr auto L   =    11;

struct M_and_T_t {
  u128 M;
  u128 T;
};

struct U_and_K_t {
  u128 U;
  u32  K;
};

u128 constexpr
pow5(u32 e) {
  if (e == 0)
    return 1;
  auto const p1 = pow5(e / 2);
  return p1 * p1 * (e % 2 == 0 ? 1 : 5);
}

u128 constexpr
pow2(u32 e) {
  return u128(1) << e;
}

s32 constexpr
log10_pow2(s32 p) {
  return 566611 * p / 1882241;
}

constexpr auto P2P = pow2(P);
constexpr auto P2L = static_cast<u32>(pow2(L));

std::ostream&
operator <<(std::ostream& o, u128 n) {
  u64 const m = n;
  return m == n ? o << m : o << "####################";
}
