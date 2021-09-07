// g++ -O3 -std=c++20 generation/generator.cpp -o generator -Wall -Wextra
// ./generator > include/table32.h

#include <climits>
#include <cstdint>
#include <iomanip>
#include <ios>
#include <iostream>
#include <fstream>
#include <string>
#include <utility>

#include <boost/multiprecision/cpp_int.hpp>

using bigint_t = __uint128_t;
//using bigint_t = boost::multiprecision::uint256_t;

// float
constexpr auto P  =   23;
constexpr auto L  =    8;
constexpr auto E0 = -149;
using     suint_t = uint32_t;

using pair_t     = std::pair<bigint_t, bigint_t>;
using affine_t   = pair_t;
using rational_t = pair_t;
using interval_t = pair_t;

rational_t
find_maximiser(affine_t const& numerator, affine_t const& denominator,
  bigint_t const& delta, interval_t const& interval) {

  bigint_t m         = interval.first;
  auto     value     = numerator   .first*m + numerator  .second;
  auto     remainder = (denominator.first*m + denominator.second)%delta;

  rational_t maximiser = { value, delta - remainder };

  while (m < interval.second) {

    value     += numerator  .first;
    remainder += denominator.first;
    while (remainder >= delta)
      remainder -= delta;

    if (value*maximiser.second > maximiser.first*(delta - remainder))
      maximiser = { value, delta - remainder };

    ++m;
  }

  return maximiser;
}

#if 0

rational_t
find_log_pow2_coefficient(unsigned base, unsigned max_exponent) {

  unsigned   p            = 0;
  bigint_t   pow_2_p      = 1; // 2^p
  rational_t coefficient  = { 0, 1 };
  unsigned   n            = 0; // floor(log_base(2^p)) = floor(coefficient*p)

  bigint_t   pow_base_np1 = 1; // base^(n + 1)

  while (p < max_exponent) {

    p       += 1;
    pow_2_p *= 2;

    if (pow_2_p >= pow_base_np1) {

      n            += 1;
      pow_base_np1 *= base;

      if (n * coefficient.denominator > coefficient.numerator * p)
        coefficient = { n, p };
    }
  };

  return coefficient;
}
#endif

struct fast_rational_t {
  bigint_t numerator;
  unsigned exponent2;
};

fast_rational_t
find_fast_coefficient(rational_t const& coefficient,
  rational_t const& maximiser, unsigned max_exponent2) {

  auto const& numerator   = coefficient.first;
  auto const& denominator = coefficient.second;

  unsigned k         = 0;
  bigint_t pow_2_k   = 1;
  bigint_t u         = numerator/denominator;
  bigint_t remainder = numerator%denominator;

  while (k < max_exponent2) {

    if (maximiser.first*(denominator - remainder) <
      pow_2_k*maximiser.second) {
      return { u + 1, k };
    }

    k         += 1;
    pow_2_k   *= 2;
    u         *= 2;
    remainder *= 2;
    while (remainder >= denominator) {
      u         += 1;
      remainder -= denominator;
    }
  }

  return { 0, 0 }; // Failed.
}

#if 0

template <typename T>
auto constexpr max = std::numeric_limits<T>::max();

struct log_pow2_t {

  log_pow2_t(unsigned base, unsigned max_eponent,
    unsigned size = max<unsigned>())
  :
  base_       {base       },
  max_eponent_{max_eponent},
  size_       {size       },
  {
    unsigned n    = 1;

    bigint_t c    = 0;
    unsigned s    = 0;
    bigint_t p2_n = 2;
    bigint_t pb_n = base;

    while (n < max_eponent) {

      auto const m = n*c >> s;

      if (2*p2_p >= pb_np1) {
        if () {
          c = 2*c;
        }
        else {
          c = 2*c + 1;
        }
        s += 1;
      }
      p    += 1;
      p2_p *= 2;
    }
  }

private:

  unsigned base_;
  unsigned max_eponent_;
  unsigned size_;
  bigint_t coefficient_;
  unsigned shift_;
};
#endif

#if 0

struct fp_params_t {

  fp_params_t(unsigned exponent_size, unsigned mantissa_size,
    unsigned min_exponent, std::string float_type, std::string uint_type,
    std::string duint_type)
    :
    size_{1 + exponent_size + mantissa_size},
    exponent_size_{exponent_size},
    mantissa_size_{mantissa_size},
    min_exponent_{min_exponent},
    max_exponent_{min_exponent + int(pow2(exponent_size)) - 2},
    large_exponent_{min_exponent},
    {

    }
  }

  unsigned size_;
  unsigned exponent_size_;
  unsigned mantissa_size_;
  int      min_exponent_;
  int      max_exponent_;
  int      large_exponent;
  unsigned word_size;
};


enum {
  exponent_size  = AMARU_EXPONENT_SIZE,
  mantissa_size  = AMARU_MANTISSA_SIZE,
  large_exponent = AMARU_LOG5_POW2(mantissa_size + 2),
  word_size      = CHAR_BIT*sizeof(suint_t),
  exponent_min   = -(1 << (exponent_size - 1)) - mantissa_size + 2
};

#define AMARU_EXPONENT_SIZE  8
#define AMARU_MANTISSA_SIZE  23
#define AMARU_FP             float
#define AMARU_SUINT          uint32_t
#define AMARU_DUINT          uint64_t
#define AMARU_REP            rep32_t
#define AMARU_TO_DECIMAL     to_decimal32
#define AMARU_TABLE          "table32.h"



// double
// constexpr auto P  =    52;
// constexpr auto E0 = -1074;
// constexpr auto L  =    11;
// using     suint_t = uint64_t;

#endif

bigint_t pow5(unsigned e) {
  if (e == 0)
    return 1;
  auto const p1 = pow5(e / 2);
  return p1 * p1 * (e % 2 == 0 ? 1 : 5);
}

bigint_t pow2(unsigned e) {
  return bigint_t{1} << e;
}

int log10_pow2(int exponent) {
  return exponent >= 0 ?
    (int) (1292913986 * ((uint64_t) exponent) >> 32) :
    -log10_pow2(-exponent) - 1;
}

auto const P2P = static_cast<suint_t>(pow2(P));
auto const P2L = static_cast<unsigned>(pow2(L));

std::ostream&
operator <<(std::ostream& o, __uint128_t n) {
  uint64_t const m = n;
  return m == n ? o << m : o << "####################";
}

//----------------------------

constexpr unsigned fixed_k = 0;

//----------------------------

rational_t
get_M_and_T(rational_t const& coefficient) {

  auto const& alpha = coefficient.first;
  auto const& delta = coefficient.second;

  auto const maximiser1 = [&]() {
    affine_t   numerator   = { 2, -1 };
    affine_t   denominator = { 2*alpha, -alpha };
    interval_t interval    = { P2P, 2*P2P + 1 };
    return find_maximiser(numerator, denominator, delta, interval);
  }();

  auto const maximiser2 = [&]() {
    affine_t   numerator   = { 4, 0 };
    affine_t   denominator = { 4*alpha, 0 };
    interval_t interval    = { P2P, 2*P2P };
    return find_maximiser(numerator, denominator, delta, interval);
  }();

  return (maximiser1.first*maximiser2.second >
    maximiser2.first*maximiser1.second) ? maximiser1 : maximiser2;
}

fast_rational_t
get_U_and_K(rational_t const& coefficient, rational_t const& M_and_T,
  unsigned /*fixed_k*/ = 0) {
  return find_fast_coefficient(coefficient, M_and_T, 1024);
}

bool check(rational_t const& coefficient, fast_rational_t const& U_and_K) {

  auto const& alpha = coefficient.first;
  auto const& delta = coefficient.second;
  auto const& U     = U_and_K.numerator;
  auto const& K     = U_and_K.exponent2;

  for (bigint_t m2 = P2P; m2 < 2*P2P; ++m2) {

    auto const m = 2*m2 - 1;
    if (m*alpha/delta != m*U >> K)
      return false;

    auto const x = 4*m2;
    if (x*alpha/delta != x*U >> K)
      return false;
  }
  return true;
}

auto const E2_max = E0 + static_cast<int>(P2L) - 2;

struct table_file_t {

  table_file_t(char const* file_name) : stream_{file_name } {

    //std::cout << "Creating ./" << file_name << '\n';

    stream_ <<
      "// This file is auto-generated. DO NOT EDIT.\n"
      "\n"
      "#include <stdint.h>\n"
      "\n"
      "typedef float    fp_t;\n"
      "typedef uint32_t suint_t;\n"
      "typedef uint64_t duint_t;\n"
      "\n"
      "typedef struct {\n"
      "  bool    negative;\n"
      "  int     exponent;\n"
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
      "\n"
      "#define AMARU_LOG10_POW2(e) ((int)(1292913986*((duint_t) e) >> 32))\n"
      "\n";
  }

  std::ofstream stream_;
};

static
void generate_scaler_params(table_file_t& file) {

  std::cerr << "E2\tF\talpha\tdelta\tM\tT\tU\tK\tCHECK\n";
  file.stream_ <<
    "static struct {\n"
    "  suint_t  const upper;\n"
    "  suint_t  const lower;\n"
    "  unsigned const n_bits;\n"
    "} scalers[] = {\n";

  for (int E2 = E0; E2 < E2_max; ++E2) {

    if (E2 <= 0)
      continue;

    auto const F           = log10_pow2(E2);
    auto const coefficient = E2 > 0
       ? rational_t{ pow2(E2 - 1 - F), pow5(F) }
       : rational_t{ pow5(F), pow2(E2 - 1 - F) };

    auto const M_and_T     = get_M_and_T(coefficient);
    auto const U_and_K     = get_U_and_K(coefficient, M_and_T, fixed_k);
    auto const CHECK       = check(coefficient, U_and_K);

    std::cerr <<
      E2                 << '\t' <<
      F                  << '\t' <<
      coefficient.first  << '\t' <<
      coefficient.second << '\t' <<
      M_and_T.first      << '\t' <<
      M_and_T.second     << '\t' <<
      U_and_K.numerator  << '\t' <<
      U_and_K.exponent2  << '\t' <<
      CHECK              << '\n';

    auto const high   = static_cast<uint32_t>(U_and_K.numerator >> 32);
    auto const low    = static_cast<uint32_t>(U_and_K.numerator);
    auto const n_bits = U_and_K.exponent2;

    file.stream_ << "  { " <<
      "0x"   << std::hex << std::setw(8) << std::setfill('0') <<
      high   << ", " <<
      "0x"   << std::hex << std::setw(8) << std::setfill('0') <<
      low    << ", " <<
      std::dec <<
      n_bits << " }, // "
      << E2 << "\n";
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

  for (int E2 = E0; E2 < E2_max; ++E2) {

    if (E2 <= 0)
      continue;

    suint_t a, b, estimate, correct;
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
      auto const delta = E2 > 0 ? pow5(F)          : pow2(-E2 + 1 + F);

      estimate = static_cast<suint_t>((4*P2P - 2)*alpha/delta);
      a_is_mid = (4*P2P - 1)%delta == 0;
      a        = static_cast<suint_t>((4*P2P - 1)*alpha/delta) +
        !a_is_mid;
      b        = static_cast<suint_t>((4*P2P + 2)*alpha/delta);
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
      correct              = static_cast<suint_t>(value/delta_adj);
      auto const vcorrect  = correct*delta_adj;
      if (value - vcorrect > vcorrect + delta_adj - value || correct < a)
        ++correct;
    }

    unsigned const correction = correct - estimate;

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
  table_file.stream_ << '\n';
  generate_corrector_params(table_file);
}
