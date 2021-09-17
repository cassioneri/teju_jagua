// g++ -O3 -std=c++11 src/generator.cpp -o generator -Wall -Wextra

#include <climits>
#include <cstdint>
#include <iomanip>
#include <ios>
#include <iostream>
#include <fstream>
#include <string>

#include <boost/multiprecision/cpp_int.hpp>

//using bigint_t = __uint128_t;
using bigint_t = boost::multiprecision::uint256_t;

// float
auto  constexpr size    = uint32_t(32);
auto  constexpr P       = uint32_t(23);
auto  constexpr L       = uint32_t(8);
auto  constexpr E0      = int32_t(-149);
using suint_t           = uint32_t;

// double
// constexpr auto P  =    52;
// constexpr auto E0 = -1074;
// constexpr auto L  =    11;
// using     suint_t = uint64_t;

auto  constexpr fixed_k = uint32_t(0);

struct affine_t {
  bigint_t slope;
  bigint_t shift;
};

struct rational_t {
  bigint_t num;
  bigint_t den;
  bool operator <(rational_t const& other) const {
    return num*other.den < den*other.num;
  }
};

struct fast_rational_t {
  bigint_t factor;
  uint32_t n_bits;
};

struct interval_t {
  bigint_t begin;
  bigint_t end;
};

rational_t inline
get_maximiser(affine_t const& num, affine_t const& den, bigint_t const& delta,
  interval_t const& interval) {

  bigint_t   m   = interval.begin;
  bigint_t   val = num.slope*m + num.shift;
  bigint_t   rem = (den.slope*m + den.shift)%delta;
  rational_t max = { val, delta - rem };

  for (++m; m < interval.end; ++m) {

    val += num.slope;
    rem += den.slope;
    while (rem >= delta)
      rem -= delta;

    rational_t new_maximiser{ val, delta - rem };
    if (max < new_maximiser)
      max = std::move(new_maximiser);
  }

  return max;
}

fast_rational_t inline
get_fast_coefficient(rational_t const& coefficient,
  rational_t const& maximiser, uint32_t /*fixed_k*/ = 0) {

  bigint_t const& num = coefficient.num;
  bigint_t const& den = coefficient.den;

  uint32_t k    = 0;
  bigint_t pow2 = 1; // 2^k
  bigint_t u    = num/den;
  bigint_t rem  = num%den;

  while (k <= 2*size) {

    if (maximiser < rational_t{pow2, den - rem})
      return { u + 1, k };

    k    += 1;
    pow2 *= 2;
    u    *= 2;
    rem  *= 2;
    while (rem >= den) {
      u   += 1;
      rem -= den;
    }
  }

  return { 0, 0 }; // Failed.
}

bigint_t pow2(uint32_t e) {
  return bigint_t(1) << e;
}

bigint_t pow5(uint32_t e) {
  if (e == 0)
    return 1;
  auto const p1 = pow5(e / 2);
  return p1 * p1 * (e % 2 == 0 ? 1 : 5);
}

int32_t log10_pow2(int32_t exponent) {
  return exponent >= 0 ?     int32_t(1292913987*uint64_t(exponent) >> 32) :
    -log10_pow2(-exponent) - 1;
}

auto const P2P = pow2(P);

inline std::ostream&
operator <<(std::ostream& o, __uint128_t n) {
  uint64_t const m = n;
  return m == n ? o << m : o << "####################";
}

rational_t
get_maximiser(rational_t const& coefficient, bool start_at_1 = false) {

  auto const& alpha = coefficient.num;
  auto const& delta = coefficient.den;

  auto const maximiser1 = [&]() {
    affine_t   num      = { 2, 1 };
    affine_t   den      = { 2*alpha, alpha };
    interval_t interval = { start_at_1 ? 1 : P2P - 1, 2*P2P };
    return get_maximiser(num, den, delta, interval);
  }();

  auto const maximiser2 = [&]() {
    affine_t   num      = { 2, 0 };
    affine_t   den      = { 2*alpha, 0 };
    interval_t interval = { start_at_1 ? 1 : P2P, 2*P2P };
    return get_maximiser(num, den, delta, interval);
  }();

  return maximiser2 < maximiser1 ? maximiser1 : maximiser2;
}

bool check(rational_t const& coefficient,
  fast_rational_t const& fast_coefficient, bool start_at_1 = false) {

  auto const& alpha  = coefficient.num;
  auto const& delta  = coefficient.den;
  auto const& factor = fast_coefficient.factor;
  auto const& n_bits = fast_coefficient.n_bits;

  for (bigint_t m2 = start_at_1 ? 1 : P2P; m2 < 2*P2P; ++m2) {

    auto const m = 2*m2 - 1;
    if (m*alpha/delta != m*factor >> n_bits)
      return false;

    auto const x = 2*m2;
    if (x*alpha/delta != x*factor >> n_bits)
      return false;
  }
  return true;
}

auto const E2_max = E0 + int32_t(uint32_t(1) << L) - 2;

struct table_file_t {

  table_file_t(char const* file_name) : stream_{file_name } {

    //std::cout << "Creating ./" << file_name << '\n';

    stream_ <<
      "// This file is auto-generated. DO NOT EDIT IT.\n"
      "\n"
      "#include <stdint.h>\n"
      "\n"
      "typedef float    fp_t;\n"
      "typedef uint32_t suint_t;\n"
      "typedef uint64_t duint_t;\n"
      "\n"
      "typedef struct {\n"
      "  bool    negative;\n"
      "  int32_t exponent;\n"
      "  suint_t mantissa;\n"
      "} rep_t;\n"
      "\n"
      "enum {\n"
      "  exponent_size  = " << L << ",\n"
      "  mantissa_size  = " << P << ",\n"
      "  large_exponent = 10,\n" // LOG5_POW2(mantissa_size + 2)"
      "  word_size      = 32,\n"
      "  exponent_min   = " << E0 << "\n"
      "};\n"
      "\n";
  }

  std::ofstream stream_;
};

static
void generate_scaler_params(table_file_t& file) {

  std::cerr << "e2\tf\talpha\tdelta\tnum\tden\tfactor\tn_bits\tCHECK\n";
  file.stream_ <<
    "static struct {\n"
    "  suint_t  const upper;\n"
    "  suint_t  const lower;\n"
    "  uint32_t const n_bits;\n"
    "} scalers[] = {\n";

  for (int32_t e2 = E0; e2 < E2_max; ++e2) {

    auto const f           = log10_pow2(e2);
    auto const e           = e2 - f;
    auto const coefficient = f >= 0
       ? rational_t{ pow2( e), pow5( f) }
       : rational_t{ pow5(-f), pow2(-e) };

    auto const start_at_1       = e2 == E0;
    auto const maximiser        = get_maximiser(coefficient, start_at_1);
    auto const fast_coefficient = get_fast_coefficient(coefficient, maximiser,
      fixed_k);
    auto const valid            = check(coefficient, fast_coefficient,
      start_at_1);

    std::cerr <<
      e2                      << '\t' <<
      f                       << '\t' <<
      coefficient.num         << '\t' <<
      coefficient.den         << '\t' <<
      maximiser.num           << '\t' <<
      maximiser.den           << '\t' <<
      fast_coefficient.factor << '\t' <<
      fast_coefficient.n_bits << '\t' <<
      valid                   << '\n';

    auto const high   = static_cast<uint32_t>(fast_coefficient.factor >> 32);
    auto const low    = static_cast<uint32_t>(fast_coefficient.factor);
    auto const n_bits = fast_coefficient.n_bits;

    file.stream_ << "  { " <<
      "0x"     << std::hex << std::setw(8) << std::setfill('0') <<
      high     << ", " <<
      "0x"     << std::hex << std::setw(8) << std::setfill('0') <<
      low      << ", " <<
      std::dec <<
      n_bits   << " }, // " <<
      e2       << "\n";
  }
  file.stream_ << "};\n";
}

static
void generate_corrector_params(table_file_t& file) {

  std::cerr << "E2\tF\tEstimate\tCorrection\tRefine\n";
  file.stream_ <<
    "static struct {\n"
    "  unsigned const char correction : " << CHAR_BIT - 1 << ";\n"
    "  unsigned const char refine     : 1;\n"
    "} correctors[] = {\n";

  for (int32_t E2 = E0; E2 < E2_max; ++E2) {

    bigint_t a, b, estimate, correct;
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

      auto const alpha = E2 > 0 ? pow2(E2 - 2 - F) : pow5(-F);
      auto const delta = E2 > 0 ? pow5(F)          : pow2(-E2 + 2 + F);

      estimate = (4*P2P - 2)*alpha/delta;
      a_is_mid = (4*P2P - 1)%delta == 0;
      a        = (4*P2P - 1)*alpha/delta + !a_is_mid;
      b        = (4*P2P + 2)*alpha/delta;
    }

    auto const c = 10*(b/10);

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
      auto const alpha_adj = E2 > 0 ? pow2(E2 - F) : pow5(-F);
      auto const delta_adj = E2 > 0 ? pow5(F)      : pow2(-E2 + F);
      auto const value     = P2P*alpha_adj;
      correct              = value/delta_adj;
      auto const vcorrect  = correct*delta_adj;
      if (value - vcorrect > vcorrect + delta_adj - value || correct < a)
        ++correct;
    }

    auto const correction = correct - estimate;

    std::cerr <<
      E2         << '\t' <<
      F          << '\t' <<
      estimate   << '\t' <<
      correction << '\t' <<
      refine     << '\n';

    file.stream_ << "  { " <<
       correction << ", " <<
       refine     << " }, // " <<
       E2 << "\n";
  }
  file.stream_ << "};\n";
}

int main() {
   auto table_file = table_file_t{"./include/table32.h"};
   generate_scaler_params(table_file);
   std::cerr << '\n';
   //table_file.stream_ << '\n';
   //generate_corrector_params(table_file);
}
