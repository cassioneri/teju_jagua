#include <cassert>
#include <cstdint>
#include <iostream>
#include <tuple>

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

std::tuple<u128, u128, u128> constexpr
get_M2_U_and_V(u128 R, u128 P5F) {

  u128 U  = 0;
  u128 V  = 1;
  u128 M2 = 0;

  u128 m2 = 0;
  u128 p  = R;
  u128 u  = 0;
  u128 v  = R;

  for (u128 n = 1; n < P2P; ++n) {

    m2 += 2;
    v  += 2*R;

    if (v >= P5F) {
      u += 1;
      v -= P5F;
    }
    if (v >= P5F) {
      u += 1;
      v -= P5F;
    }

    // Alternatively, we can check m2 * V > M2 * v. This does change
    // the return values because m2 and v changes at each iteration but
    // u might stall for a few ones.
    if (u * V > U * v) {
      U  = u;
      V  = v;
      M2 = m2;
    }
  }
  return {M2, U, V};
}

std::pair<u128, u32> constexpr
get_M_and_K(u128 M2, u128 V, u128 R, u128 P5F) {

  u128 p2k = 1;
  u128 m   = 0;
  u128 s   = R;

  for (u32 k = 0; k < 128; ++k) {

    if (p2k * V >= s * (M2 + 1))
      return {m, k};

    p2k *= 2;
    m   *= 2;
    s   *= 2;

    if (s >= P5F) {
      m += 1;
      s -= P5F;
    }
    if (s >= P5F) {
      m += 1;
      s -= P5F;
    }
  }

  return {0, 0};
}

bool check(u128 M, u32 K, u128 R, u128 P5F) {
  for (u32 n2 = 0; n2 < P2P; ++n2) {
    auto const m2p1 = 2*n2 + 1;
    if (m2p1*R/P5F != m2p1*M >> K)
      return false;
  }
  return true;
}

int main() {

  std::cout << "E1\tF\t5^F\tE2\t2^E2-F\tQ\tR\tM2\tU\tV\tM\tK\tCHECK\n";

  for (int E1 = E1_min; E1 <= E1_max; ++E1) {

    auto const F          = log10_pow2(E1 - P);

    if (F <= 0)
      continue;

    auto const P5F        = pow5(F);

    if (P5F < 2*P2P)
      continue;

    auto const E2         = E1 - P - 1;
    auto const P2E2MF     = pow2(E2 - F);

    if (P5F < 2*P2P)
      continue;

    auto const Q          = P2E2MF / P5F;
    auto const R          = P2E2MF % P5F;
    auto const [M2, U, V] = get_M2_U_and_V(R, P5F);
    auto const [M,  K]    = get_M_and_K(M2, V, R, P5F);

    auto const M0_MAX     = (2*P2P - 1);
    auto const CHECK      = check(M, K, R, P5F);

    std::cout <<
      E1     << '\t' <<
      F      << '\t' <<
      P5F    << '\t' <<
      E2     << '\t' <<
      P2E2MF << '\t' <<
      Q      << '\t' <<
      R      << '\t' <<
      M2     << '\t' <<
      U      << '\t' <<
      V      << '\t' <<
      M      << '\t' <<
      K      << '\t' <<
      CHECK  << '\n';
  }
  return 0;
}
