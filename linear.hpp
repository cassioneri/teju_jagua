#include <tuple>
#include <utility>

#include "common.hpp"

namespace linear {

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
get_M_and_K(u128 R, u128 P5F) {

  auto const [M2, U, V] = get_M2_U_and_V(R, P5F);

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

} // namespace linear
