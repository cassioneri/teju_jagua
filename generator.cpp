// g++ -O3 -std=c++20 generator.cpp -o generator

#include <cstdint>
#include <iomanip>
#include <ios>
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

struct params_t {
  u32  Q;
  u128 U;
  u32  K;
};

struct rep32_t {
  u32 mantissa;
  u32 exponent;
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

// ----------------------------

constexpr bool is_debug = false;
constexpr u32  fixed_k  = 56;

// ----------------------------

namespace linear {

M_and_T_t constexpr
get_M_and_T(u128 R, u128 P5F) {

  u128 m = 2*P2P - 1;
  u128 t = m*R % P5F;

  u128 M = m;
  u128 T = t;

  u128 x = 4*P2P;
  u128 y = x*R % P5F;

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

    if (x * T > M * y) {
      M  = x;
      T  = y;
    }

    x += 4;
    y += 4*R;
    while (y >= P5F)
      y -= P5F;
  }
  return {M, T};
}

U_and_K_t constexpr
get_U_and_K(u128 R, u128 P5F, M_and_T_t M_and_T, u32 fixed_k = 0) {

  u128 p2k = 1;
  u128 u   = 0;
  u128 v   = R;

  for (u32 k = 0; k < 128; ++k) {

    if ((fixed_k == 0 && p2k * M_and_T.T >= v * M_and_T.M) ||
      (fixed_k != 0 && k == fixed_k))
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

    auto const x = 4*m2;
    if (x*R/P5F != x*U_and_K.U >> U_and_K.K)
      return false;
  }
  return true;
}

} // namespace linear

int main() {

  if (is_debug)
    std::cout << "E2\tF\t5^F\tE\t2^E\tQ\tR\tM\tT\tU\tK\tCHECK\n";

  else
    std::cout <<
      "// This file is auto-generated. DO NOT EDIT.\n"
      "\n"
      "#include \"config32.h\"\n"
      "\n"
      "AMARU_DOUBLE params[] = {\n";

  constexpr auto E2_max = E0 + static_cast<int>(P2L);

  for (int E2 = E0; E2 <= E2_max; ++E2) {

    if (E2 <= 0)
      continue;

    auto const F   = log10_pow2(E2);
    auto const P5F = pow5(F);

    if (P5F <= 4*P2P)
      continue;

    auto const E       = E2 - 1 - F;
    auto const P2E     = pow2(E);
    auto const Q       = P2E / P5F;
    auto const R       = P2E % P5F;
    auto const M_and_T = linear::get_M_and_T(R, P5F);
    auto const U_and_K = linear::get_U_and_K(R, P5F, M_and_T, fixed_k);
    auto const CHECK   = linear::check(R, P5F, U_and_K);

    if (is_debug)
      std::cout <<
        E2        << '\t' <<
        F         << '\t' <<
        P5F       << '\t' <<
        E         << '\t' <<
        P2E       << '\t' <<
        Q         << '\t' <<
        R         << '\t' <<
        M_and_T.M << '\t' <<
        M_and_T.T << '\t' <<
        U_and_K.U << '\t' <<
        U_and_K.K << '\t' <<
        CHECK     << '\n';

    else
      std::cout << std::hex << "    0x" << std::setw(16) <<
        std::setfill('0') << (u128(Q) << U_and_K.K) + U_and_K.U << ",\n";
  }

  if (!is_debug)
    std::cout << "};\n";

  return 0;
}
