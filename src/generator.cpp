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

auto constexpr fixed_k = uint32_t(0);

/**
 * \brief Rational number p / q.
 */
struct rational_t {

  rational_t(bigint_t p, bigint_t q) : p(std::move(p)), q(std::move(q)) {
    auto const x = gcd(this->p, this->q);
    this->p /= x;
    this->q /= x;
  }

  bool operator <(rational_t const& other) const {
    return p * other.q < q * other.p;
  }

  bigint_t p;
  bigint_t q;
};

/**
 * \brief Fast EAF coefficients.
 *
 * For fixed alpha > 0 and delta > 0, we often find U > 0 and k >= 0 such that
 *     alpha * m / delta = U * m >> k for m in a certain interval.
 *
 * This type stores U and lk
 */
struct fast_eaf_t {
  bigint_t U;
  uint32_t k;
};

/**
 * \brief Integer interval [a, b[.
 */
struct interval_t {
  bigint_t a;
  bigint_t b;
};

/**
 * \brief Returns 2^e.
 */
bigint_t pow2(uint32_t e) {
  return bigint_t(1) << e;
}

/**
 * \brief Returns 5^e.
 */
bigint_t pow5(uint32_t e) {
  if (e == 0)
    return 1;
  auto const p1 = pow5(e / 2);
  return p1 * p1 * (e % 2 == 0 ? 1 : 5);
}

/**
 * \brief Returns log_10(2^e).
 */
int32_t log10_pow2(int32_t e) {
  return int32_t(1292913987 * uint64_t(e) >> 32);
}

/**
 * \brief Meta information about the floating point number type of interest.
 *
 * More precisely:
 *
 * fp_t    : The floating point number type of interest.
 * suint_t : Unsigned integer type such that sizeof(suint_t) >= sizeof(fp_t).
 * duint_t : Unsigned integer type such that sizeof(duint_t) >= 2*sizeof(suint_t).
 */
struct fp_type_t {

  /**
   * \brief Constructor.
   *
   * \param name       C/C++ name of fp_t (e.g., "float" or "double").
   * \param suint_name C/C++ name of suint_t (e.g., "uint32_t" or "uint64_t").
   * \param duint_name C/C++ name of duint_t (e.g., "uint64_t" or "uint128_t").
   * \param P          Number of mantissa bits.
   * \param L          Number of exponent bits.
   * \param E0         Minimum exponent. (As described in Amaru's
   *                   representation.)
   */
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

  /**
   * \brief Returns the C/C++ name of fp_t.
   */
  std::string const&
  name() const {
    return name_;
  }

  /**
   * \brief Returns the C/C++ name of suint_t.
   */
  std::string const&
  suint_name() const {
    return suint_name_;
  }

  /**
   * \brief Returns the C/C++ name of duint_t.
   */
  std::string const&
  duint_name() const {
    return duint_name_;
  }

  /**
   * \brief Returns the number of mantissa bits.
   */
  uint32_t
  P() const {
    return P_;
  }

  /**
   * \brief Returns the number of exponent bits.
   */
  uint32_t
  L() const {
    return L_;
  }

  /**
   * \brief Returns the minimum exponent. (As described in Amaru's
   * representation.)
   */
  int32_t
  E0() const {
    return E0_;
  }

  /**
   * \brief Returns the size in bits of ft_p.
   */
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

/**
 * \brief Amaru's table generator.
 */
struct generator_t {

  /**
   * \brief Constructor.
   *
   * \param fp_type The meta information of the floating point number type
   *                whose table shall be generated.
   */
  generator_t(fp_type_t fp_type) :
    fp_type_{std::move(fp_type)},
    P2P_    {pow2(fp_type_.P())} {
  }

  /**
   * \brief Generates the table.
   *
   * \brief stream The output stream to receive the table.
   */
  void
  generate(std::ostream& stream) const {
    std::cout << "Generating...\n";
    std::cout << "  Header...\n";
    append_header (stream);
    std::cout << "  Scalers table...\n";
    append_scalers(stream);
  }

private:

  /**
   * \brief Streams out the table header.
   *
   * \brief stream The output stream.
   */
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

  /**
   * \brief Streams out the scalers.
   *
   * \brief stream The output stream.
   */
  void
  append_scalers(std::ostream& stream) const {

    std::cerr << "e2\tf\talpha\tdelta\tnum\tden\tfactor\tshift\n";
    stream <<
      "static struct {\n"
      "  suint_t  const upper;\n"
      "  suint_t  const lower;\n"
      "  uint32_t const shift;\n"
      "} scalers[] = {\n";

    auto const E2_max = fp_type_.E0() +
      int32_t(uint32_t(1) << fp_type_.L()) - 2;

    //int32_t e2 = -24; {
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

      std::cerr <<
        e2                 << '\t' <<
        f                  << '\t' <<
        coefficient.p      << '\t' <<
        coefficient.q      << '\t' <<
        maximiser.p        << '\t' <<
        maximiser.q        << '\t' <<
        fast_coefficient.U << '\t' <<
        fast_coefficient.k << '\n';

      auto const upper = static_cast<uint32_t>(fast_coefficient.U >> 32);
      auto const lower   = static_cast<uint32_t>(fast_coefficient.U);
      auto const shift = fast_coefficient.k;

      stream << "  { " <<
        "0x"     << std::hex << std::setw(8) << std::setfill('0') <<
        upper    << ", " <<
        "0x"     << std::hex << std::setw(8) << std::setfill('0') <<
        lower    << ", " <<
        std::dec <<
        shift    << " }, // " <<
        e2       << "\n";
    }
    stream << "};\n";
  }

  rational_t static
  get_maximiser(bigint_t alpha, bigint_t const& delta, bigint_t const& a,
    bigint_t const& b) {

    static int level = 0;
    ++level;

    auto const b_minus_1 = b - 1;
    auto const phi_0     = rational_t{b_minus_1,
      delta - alpha * b_minus_1 % delta};

    if (a == b_minus_1)
      return --level, phi_0;

    if (alpha >= delta)
      alpha = alpha % delta;

    if (alpha == 0)
      return --level, rational_t{b_minus_1, delta};

    auto const  a1 = alpha * a / delta + 1;
    auto const  b1 = alpha * b_minus_1 / delta + 1;

    if (a1 == b1)
      return --level, phi_0;

    auto const& delta1 = alpha;
    auto const  alpha1 = delta1 - delta % delta1;
    auto const  phi_1  = get_maximiser(alpha1, delta1, a1, b1);

    auto maximizer = rational_t{delta * phi_1.p - phi_1.q, delta1 * phi_1.q};

    return std::max(maximizer, phi_0);
  }

  rational_t
  get_maximiser(rational_t const& coefficient, bool start_at_1 = false) const {

    auto const& delta = coefficient.q;
    auto const  alpha = coefficient.p % delta;

    bigint_t const a      = start_at_1 ? 1 : 2 * P2P_;
    bigint_t const b      = 4 * P2P_;
    auto const maximiser1 = get_maximiser(alpha, delta, a, b);

    bigint_t   const m2         = 20 * P2P_;
    bigint_t   const r2         = m2 * alpha % delta;
    rational_t const maximiser2 = { m2, delta - r2 };

    return std::max(maximiser1, maximiser2);
  }

  fast_eaf_t
  get_fast_coefficient(rational_t const& coefficient,
    rational_t const& maximiser, uint32_t /*fixed_k*/ = 0) const {

    bigint_t const& num = coefficient.p;
    bigint_t const& den = coefficient.q;

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
  auto generator = generator_t{float_config};
  auto stream    = std::ofstream{"./include/table32.h"};

//  auto double_config   = fp_type_t{
//    /* name       */ "double",
//    /* suint_name */ "uint64_t",
//    /* suint_name */ "__uint128_t",
//    /* P          */ 52,
//    /* L          */ 11,
//    /* E0         */ -1074
//  };
//  auto generator = generator_t{double_config};
//  auto stream    = std::ofstream{"./include/table64.h"};

  generator.generate(stream);
}
