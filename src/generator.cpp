// g++ -O3 -std=c++11 src/generator.cpp -o generator -Wall -Wextra

#include <algorithm>
#include <climits>
#include <cstdint>
#include <iomanip>
#include <ios>
#include <iostream>
#include <fstream>
#include <string>

#include <boost/multiprecision/cpp_int.hpp>

using bigint_t = boost::multiprecision::uint256_t;

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
  return int32_t(1292913987 * uint64_t(exponent) >> 32);
}

struct fp_type_t {

  fp_type_t(std::string name, std::string suint_name, std::string duint_name,
    uint32_t P, uint32_t L, int32_t E0) :
    name_      {std::move(name)      },
    suint_name_{std::move(suint_name)},
    duint_name_{std::move(duint_name)},
    P_         {P                    },
    L_         {L                    },
    E0_        {E0                   },
    size_      { 1 + L + P           } {
    }

  std::string const&
  name() const {
    return name_;
  }

  std::string const&
  suint_name() const {
    return suint_name_;
  }

  std::string const&
  duint_name() const {
    return duint_name_;
  }

  uint32_t
  P() const {
    return P_;
  }

  uint32_t
  L() const {
    return L_;
  }

  int32_t
  E0() const {
    return E0_;
  }

  uint32_t
  size() const {
    return size_;
  }

private:

  std::string name_;
  std::string suint_name_;
  std::string duint_name_;

  uint32_t P_;
  uint32_t L_;
  int32_t  E0_;
  uint32_t size_;
};

struct generator_t {

  generator_t(fp_type_t fp_type) :
    fp_type_{std::move(fp_type)},
    P2P_    {pow2(fp_type_.P())} {
  }

  void
  generate(std::ostream& stream) const {
    std::cout << "Generating...\n";
    std::cout << "  Header...\n";
    append_header (stream);
    std::cout << "  Scalers table...\n";
    append_scalers(stream);
  }

private:

  void
  append_header(std::ostream& stream) const {
    stream <<
      "// This file is auto-generated. DO NOT EDIT IT.\n"
      "\n"
      "#include <stdint.h>\n"
      "\n"
      "typedef " << fp_type_.name()       << " fp_t;\n"
      "typedef " << fp_type_.suint_name() << " suint_t;\n"
      "typedef " << fp_type_.duint_name() << " duint_t;\n"
      "\n"
      "typedef struct {\n"
      "  bool    negative;\n"
      "  int32_t exponent;\n"
      "  suint_t mantissa;\n"
      "} rep_t;\n"
      "\n"
      "enum {\n"
      "  exponent_size  = " << fp_type_.L() << ",\n"
      "  mantissa_size  = " << fp_type_.P() << ",\n"
      "  large_exponent = 10,\n" // LOG5_POW2(mantissa_size + 2)"
      "  word_size      = " << fp_type_.size() << ",\n"
      "  exponent_min   = " << fp_type_.E0()   << "\n"
      "};\n"
      "\n";
  }

  void
  append_scalers(std::ostream& stream) const {

    std::cerr << "e2\tf\talpha\tdelta\tnum\tden\tfactor\tn_bits\tcheck\n";
    stream <<
      "static struct {\n"
      "  suint_t  const upper;\n"
      "  suint_t  const lower;\n"
      "  uint32_t const n_bits;\n"
      "} scalers[] = {\n";

    auto const E2_max = fp_type_.E0() +
      int32_t(uint32_t(1) << fp_type_.L()) - 2;

    for (int32_t e2 = fp_type_.E0(); e2 < E2_max; ++e2) {

      auto const f           = log10_pow2(e2);
      auto const e           = e2 - f;
      auto const coefficient = f >= 0
         ? rational_t{ pow2( e), pow5( f) }
         : rational_t{ pow5(-f), pow2(-e) };

      auto const start_at_1       = e2 == fp_type_.E0();
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

      stream << "  { " <<
        "0x"     << std::hex << std::setw(8) << std::setfill('0') <<
        high     << ", " <<
        "0x"     << std::hex << std::setw(8) << std::setfill('0') <<
        low      << ", " <<
        std::dec <<
        n_bits   << " }, // " <<
        e2       << "\n";
    }
    stream << "};\n";
  }

  rational_t static
  get_affine_maximiser_linear_search(affine_t const& num, affine_t const& den,
    bigint_t const& delta, interval_t const& interval) {

    bigint_t   m   = interval.begin;
    bigint_t   val = num.slope*m + num.shift;
    bigint_t   rem = (den.slope*m + den.shift) % delta;
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

  rational_t
  get_maximiser(rational_t const& coefficient, bool start_at_1 = false) const {

    auto const& alpha = coefficient.num;
    auto const& delta = coefficient.den;

    auto const maximiser1 = [&]() {
      affine_t   num      = { 1, 0 };
      affine_t   den      = { alpha, 0 };
      interval_t interval = { start_at_1 ? 1 : 2*P2P_, 4*P2P_ };
      return get_affine_maximiser_linear_search(num, den, delta, interval);
    }();

    bigint_t   const m2         = 20 * P2P_;
    bigint_t   const r2         = m2 * alpha % delta;
    rational_t const maximiser2 = { m2, delta - r2 };

    return std::max(maximiser1, maximiser2);
  }

  fast_rational_t
  get_fast_coefficient(rational_t const& coefficient,
    rational_t const& maximiser, uint32_t /*fixed_k*/ = 0) const {

    bigint_t const& num = coefficient.num;
    bigint_t const& den = coefficient.den;

    uint32_t k    = 0;
    bigint_t pow2 = 1; // 2^k
    bigint_t u    = num / den;
    bigint_t rem  = num % den;

    while (k <= 2*fp_type_.size()) {

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

  bool
  check(rational_t const& coefficient, fast_rational_t const& fast_coefficient,
    bool start_at_1 = false) const {

    auto const& alpha  = coefficient.num;
    auto const& delta  = coefficient.den;
    auto const& factor = fast_coefficient.factor;
    auto const& n_bits = fast_coefficient.n_bits;

    for (bigint_t m2 = start_at_1 ? 1 : P2P_; m2 < 2 * P2P_; ++m2) {

      auto const m = 2 * m2 - 1;
      if (m * alpha / delta != m * factor >> n_bits)
        return false;

      auto const x = 2 * m2;
      if (x * alpha / delta != x * factor >> n_bits)
        return false;
    }
    return true;
  }

  fp_type_t fp_type_;
  bigint_t  P2P_;
};

int main() {

  auto float_config = fp_type_t{
    /* name       */ "float",
    /* suint_name */ "uint32_t",
    /* suint_name */ "uint64_t",
    /* P          */ 23,
    /* L          */ 8,
    /* E0         */ -149
  };

//  auto double_config   = fp_type_t{
//    /* name       */ "double",
//    /* suint_name */ "uint64_t",
//    /* suint_name */ "__uint128_t",
//    /* P          */ 52,
//    /* L          */ 11,
//    /* E0         */ -1074
//  };

  auto generator = generator_t{float_config};
  auto stream    = std::ofstream{"./include/table32.h"};
  generator.generate(stream);
}
