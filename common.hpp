#include <cstdint>
#include <iostream>

using s32  = int32_t;
using u32  = uint32_t;
using u64  = uint64_t;
using u128 = __uint128_t;

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

constexpr auto P      =   23;
constexpr auto E1_min = -126;
constexpr auto E1_max =  127;
constexpr auto P2P    = pow2(P);

std::ostream&
operator <<(std::ostream& o, u128 n) {
  u64 const m = n;
  return m == n ? o << m : o << "####################";
}
