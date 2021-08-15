#include <tuple>
#include <utility>

#include "common.hpp"

namespace linear {

M_and_T_t constexpr
get_M_and_T(u128 R, u128 P5F) {

  u128 m = 2*P2P - 1;
  u128 t = m*R % P5F;

  u128 M = m;
  u128 T = t;

  for (u128 m0 = P2P; m0 < 2*P2P; ++m0) {

    // m = 2*m0 + 1
    m  += 2;

    // t = m % 5^F
    t  += 2*R;
    while (t >= P5F)
      t -= P5F;

    if (m * T > M * t) {
      M  = m;
      T  = t;
    }
  }
  return {M, T};
}

U_and_K_t constexpr
get_U_and_K(u128 R, u128 P5F, M_and_T_t M_and_T) {

  u128 p2k = 1;
  u128 u   = 0;
  u128 v   = R;

  for (u32 k = 0; k < 128; ++k) {

    if (p2k * M_and_T.T >= v * M_and_T.M)
      return {u, k};

    p2k *= 2;
    u   *= 2;
    v   *= 2;

    while (v >= P5F) {
      u += 1;
      v -= P5F;
    }
  }

  return {0, 0};
}

bool check(u128 R, u128 P5F, U_and_K_t U_and_K) {
  for (u32 m2 = P2P; m2 < 2*P2P; ++m2) {
    auto const m = 2*m2 - 1;
    if (m*R/P5F != m*U_and_K.U >> U_and_K.K)
      return false;
  }
  return true;
}

} // namespace linear
