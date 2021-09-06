// g++ -O3 -std=c++20 generation/generator.cpp -o generator -Wall -Wextra
// ./generator > include/table32.h

#include <cstdint>
#include <iomanip>
#include <ios>
#include <iostream>

using s32  = int32_t;
using u32  = uint32_t;
using u64  = uint64_t;
using u128 = __uint128_t;

// float
constexpr auto P  =   23;
constexpr auto E0 = -149;
constexpr auto L  =    8;

// double
// constexpr auto P  =    52;
// constexpr auto E0 = -1074;
// constexpr auto L  =    11;

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

inline static
int log10_pow2(int exponent) {
  return exponent >= 0 ?
    (int) (1292913986 * ((uint64_t) exponent) >> 32) :
    log10_pow2(-exponent) - 1;
}

constexpr auto P2P = pow2(P);
constexpr auto P2L = static_cast<u32>(pow2(L));

std::ostream&
operator <<(std::ostream& o, u128 n) {
  u64 const m = n;
  return m == n ? o << m : o << "####################";
}

//----------------------------

constexpr u32  fixed_k = 0;

//----------------------------

M_and_T_t constexpr
get_M_and_T(u128 P2E, u128 P5F) {

  u128 m = 2*P2P - 1;
  u128 t = m*P2E % P5F;

  u128 M = m;
  u128 T = t;

  u128 x = 4*P2P;
  u128 y = x*P2E % P5F;

  for (u128 m0 = P2P; m0 < 2*P2P; ++m0) {

    // m = 2*m0 + 1
    m  += 2;

    // t = m%5^F
    t  += 2*P2E;
    while (t >= P5F)
      t -= P5F;

    if (m * (P5F - T) > M * (P5F - t)) {
      M  = m;
      T  = t;
    }

    if (x * (P5F - T) > M * (P5F - y)) {
      M  = x;
      T  = y;
    }

    // x = 4*m0
    x += 4;

    // y = 4*m0 % P5F
    y += 4*P2E;
    while (y >= P5F)
      y -= P5F;
  }
  return {M, P5F - T};
}

U_and_K_t constexpr
get_U_and_K(u128 P2E, u128 P5F, M_and_T_t M_and_T, u32 fixed_k = 0) {

  u128 const Q = (2*P2P -1)/P5F;

  // k = 0.
  u128 p2k = 1;       // 2^k
  u128 u   = P2E/P5F; // 2^(E + k)/5^F
  u128 v   = P2E%P5F; // 2^(E + k)%5^F

  for (u32 k = 0; k < 128; ++k) {

    if (fixed_k == 0) {
      if (p2k * M_and_T.T >= (P5F - v) * M_and_T.M && u >= Q*(P5F - v))
        return {u + 1, k};
    }
    else {
      if (k == fixed_k)
        return {u + 1, k};
    }

    p2k *= 2;
    u   *= 2;
    v   *= 2;

    while (v > P5F) {
      u += 1;
      v -= P5F;
    }
  }

  return {0, 0};
}

bool check(u128 P2E, u128 P5F, U_and_K_t U_and_K) {

  for (u32 m2 = P2P; m2 < 2*P2P; ++m2) {

    auto const m = 2*m2 - 1;
    if (m*P2E/P5F != m*U_and_K.U >> U_and_K.K)
      return false;

    auto const x = 4*m2;
    if (x*P2E/P5F != x*U_and_K.U >> U_and_K.K)
      return false;
  }
  return true;
}

constexpr auto E2_max = E0 + static_cast<int>(P2L) - 2;

static
void generate_scaler_params() {

  std::cerr << "E2\tF\t2^E\t5^F\tM\tT\tU\tK\tCHECK\n";
  std::cout <<
    "static struct {\n"
    "  suint_t  const upper;\n"
    "  suint_t  const lower;\n"
    "  unsigned const n_bits;\n"
    "} scalers[] = {\n";

  for (int E2 = E0; E2 <= E2_max; ++E2) {

    if (E2 <= 0)
      continue;

    auto const F   = log10_pow2(E2);
    auto const P5F = pow5(F);

    auto const P2E     = pow2(E2 - 1 - F);
    auto const M_and_T = get_M_and_T(P2E, P5F);
    auto const U_and_K = get_U_and_K(P2E, P5F, M_and_T, fixed_k);
    auto const CHECK   = check(P2E, P5F, U_and_K);

    std::cerr <<
      E2        << '\t' <<
      F         << '\t' <<
      P2E       << '\t' <<
      P5F       << '\t' <<
      M_and_T.M << '\t' <<
      M_and_T.T << '\t' <<
      U_and_K.U << '\t' <<
      U_and_K.K << '\t' <<
      CHECK     << '\n';

    u32 const high   = U_and_K.U >> 32;
    u32 const low    = U_and_K.U;
    u32 const n_bits = U_and_K.K;

    std::cout << "  { " <<
      "0x"   << std::hex << std::setw(8) << std::setfill('0') <<
      high   << ", " <<
      "0x"   << std::hex << std::setw(8) << std::setfill('0') <<
      low    << ", " <<
      std::dec <<
      n_bits << " },\n";
  }
  std::cout << "};\n";
}

static
void generate_corrector_params() {

  std::cerr << "E2\tF\tEstimate\tCorrection\tRefine\n";
  std::cout <<
    "static struct {\n"
    "  unsigned const correction;\n"
    "  bool     const refine;\n"
    "} correctors[] = {\n";

  for (int E2 = E0; E2 < E2_max; ++E2) {

    //if (E2 != 64)
    if (E2 <= 0)
      continue;

    u32 a, b, estimate, correct;
    bool a_is_mid, shorten, refine;

    auto F = log10_pow2(E2);

    if (E2 == 0) {
      estimate = P2P - 1;
      a_is_mid = false;
      a        = P2P;
      b        = P2P;
    }

    else if (E2 == 1) {
      estimate = 2*P2P - 1;
      a_is_mid = false;
      a        = 2*P2P;
      b        = 2*P2P + 1;
    }

    else {

      auto const P5F = pow5(F);
      auto const P2E = pow2(E2 - 2 - F);
      estimate       = (4*P2P - 2)*P2E/P5F;
      a_is_mid       = (4*P2P - 1)%P5F == 0;
      a              = (4*P2P - 1)*P2E/P5F + !a_is_mid;
      b              = (4*P2P + 2)*P2E/P5F;
    }

    u32 const c = 10*(b/10);

    if (b < a) {
      refine    = true;
      F        -= 1;
      estimate *= 10;
      shorten   = false;
    }
    else {
      refine    = false;
      shorten   = c == b || c >= a;
    }

    if (shorten)
      correct = c;

    else{
      auto const P5F      = pow5(F);
      auto const value    = P2P*pow2(E2 - F);
      correct             = value/P5F;
      auto const vcorrect = correct*P5F;
      if (value - vcorrect > vcorrect + P5F - value || correct < a)
        ++correct;
    }

    unsigned const correction = correct - estimate;

    std::cerr <<
      E2         << '\t' <<
      F          << '\t' <<
      estimate   << '\t' <<
      correction << '\t' <<
      refine     << '\n';

    std::cout << "  { " <<
       correction << ", " <<
       refine     << " },\n";
  }
  std::cout << "};\n";
}

int main() {
  std::cout << "// This file is auto-generated. DO NOT EDIT.\n\n";
  generate_scaler_params();
  std::cerr << '\n';
  std::cout << '\n';
  generate_corrector_params();
}
