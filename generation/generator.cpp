// g++ -O3 -std=c++20 generation/generator.cpp -o generator

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

// s32 constexpr
// log10_pow2(s32 p) {
//   return 566611 * p / 1882241;
// }

int log10_pow2(int exponent) {
  return exponent >= 0 ?
    (int) (((uint64_t) 1292913986) * ((uint64_t) exponent) >> 32) :
    log10_pow2(-exponent) - 1;
}

unsigned remove_trailing_zeros(u32& value) {
  unsigned count = 0;
  do {
    ++count;
    value /= 10;
  } while (value % 10 == 0);
  return count;
}

constexpr auto P2P = pow2(P);
constexpr auto P2L = static_cast<u32>(pow2(L));

std::ostream&
operator <<(std::ostream& o, u128 n) {
  u64 const m = n;
  return m == n ? o << m : o << "####################";
}

// ----------------------------

constexpr bool is_debug = true;
constexpr u32  fixed_k  = 56;

// ----------------------------

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

    // x = 4*m0
    x += 4;

    // y = 4*m0 % P5F
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

constexpr auto E2_max = E0 + static_cast<int>(P2L);

void generate_multipliers_table() {

  if (is_debug)
    std::cout << "E2\tF\t5^F\tE\t2^E\tQ\tR\tM\tT\tU\tK\tCHECK\n";

  else
    std::cout <<
      "// This file is auto-generated. DO NOT EDIT.\n"
      "\n"
      "#include \"config32.h\"\n"
      "\n"
      "AMARU_DOUBLE params[] = {\n";

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
    auto const M_and_T = get_M_and_T(R, P5F);
    auto const U_and_K = get_U_and_K(R, P5F, M_and_T, fixed_k);
    auto const CHECK   = check(R, P5F, U_and_K);

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
}

void generate_correction_table() {

  if (is_debug)
    std::cout << "E2\tF\tExp\tEst\t Corr\n";
  else
    std::cout <<
      "// This file is auto-generated. DO NOT EDIT.\n"
      "\n"
      "AMARU_SINGLE correction[] = {\n";

  constexpr auto E2_max = E0 + static_cast<int>(P2L);
  constexpr auto m2 = P2P;

  int E2 = 44;
  for (int E2 = E0; E2 <= E2_max; ++E2) {

    if (E2 <= 0)
      continue;

    auto const F   = log10_pow2(E2);
    auto const P5F = pow5(F);

    if (P5F <= 4*P2P)
      continue;

    auto P2E2_2_F = pow2(E2 - 2 - F);

    int exponent = F;

    // Mantissa estimate
    u32 estimate;
    u32 const a = (4*m2 - 2)*P2E2_2_F/P5F + 1;
    u32 const b = (4*m2 + 2)*P2E2_2_F/P5F;
    u32 const c = 10*(b/10);
    if (a <= c) {
      std::cout << 'd';
      estimate = c;
    }
    else if (a % 2 == b % 2) {
      std::cout << 'm';
      estimate = (a + b)/2;
    }
    else {
      std::cout << 'c';
      u32 d = 8*m2*P2E2_2_F/P5F;
      estimate = (a + b)/2 + (d & 1);
    }

    // Mantissa correct
    u32 correct;
    u32 ac = (4*m2 - 1)*P2E2_2_F/P5F + 1;

    if (ac <= c)
      correct = c;
    else if (b >= ac) {
      auto const x = m2*pow2(E2 - F);
      correct = b;
      auto vc = b*P5F;
      while (vc >= x) {
        --correct;
        vc -= P5F;
      }
      if (x - vc > (vc + P5F) - x || correct < ac) {
        ++correct;
        // vc += P5F;
      }
    }
    else { // b < ac
      auto const P5F_1 = P5F/5;
      auto const x = m2*pow2(E2 - F + 1);
      --exponent;
      estimate *= 10;
      correct = estimate;
      auto vc = correct*P5F_1;
      while (vc < x) {
        ++correct;
        vc += P5F_1;
      }
      if (vc - x > x - (vc - P5F_1)) {
        --correct;
        // vc -= P5F;
      }
      correct += 10;
    }
    u32 correction = correct - estimate;

    if (is_debug)
      std::cout <<
        E2         << '\t' <<
        F          << '\t' <<
        exponent   << '\t' <<
        estimate   << '\t' <<
        correction << '\n';
    else
      std::cout <<
        correction << ", ";
  }

  if (!is_debug)
    std::cout << "};\n";
}

int main() {
  generate_correction_table();
}
