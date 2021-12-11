// g++ -O3 -std=c++11 src/generator.cpp -o generator -Wall -Wextra

#include <boost/multiprecision/cpp_int.hpp>

#include <algorithm>
#include <climits>
#include <cstdint>
#include <iomanip>
#include <ios>
#include <iostream>
#include <fstream>
#include <string>

using integer_t   = boost::multiprecision::cpp_int;
using rational_t = boost::multiprecision::cpp_rational;

auto constexpr fixed_k = uint32_t{0};

/**
 * \brief Fast EAF coefficients.
 *
 * For given alpha > 0 and delta > 0, we often find U > 0 and k >= 0 such that
 *     alpha * m / delta = U * m >> k for m in a certain interval.
 *
 * This type stores U and k.
 */
struct fast_eaf_t {
  integer_t U;
  uint32_t k;
};

/**
 * \brief Returns 2^e.
 */
integer_t pow2(uint32_t e) {
  return integer_t{1} << e;
}

/**
 * \brief Returns 5^e.
 */
integer_t pow5(uint32_t e) {
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
   * \param name            C/C++ name of fp_t (e.g., "float" or "double").
   * \param suint_name      C/C++ name of suint_t (e.g., "uint32_t" or
   *                        "uint64_t").
   * \param duint_name      C/C++ name of duint_t (e.g., "uint64_t" or
   *                        "uint128_t").
   * \param mantissa_size   Size of mantissa in bits.
   * \param exponent_size   Size of exponent in bits.
   * \param exponent_min    Minimum exponent. (As described in Amaru's
   *                        representation.)
   */
  fp_type_t(std::string name, std::string suint_name, std::string duint_name,
    uint32_t mantissa_size, uint32_t exponent_size, int32_t exponent_min) :
    name_         {std::move(name)                   },
    suint_name_   {std::move(suint_name)             },
    duint_name_   {std::move(duint_name)             },
    mantissa_size_{mantissa_size                     },
    exponent_size_{exponent_size                     },
    exponent_min_ {exponent_min                      },
    size_         { 1 + exponent_size + mantissa_size} {
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
   * \brief Returns the size of mantissa in bits.
   */
  uint32_t
  mantissa_size() const {
    return mantissa_size_;
  }

  /**
   * \brief Returns size of exponent in bits.
   */
  uint32_t
  exponent_size() const {
    return exponent_size_;
  }

  /**
   * \brief Returns the minimum exponent. (As described in Amaru's
   * representation.)
   */
  int32_t
  exponent_min() const {
    return exponent_min_;
  }

  /**
   * \brief Returns the size of ft_p in bits.
   */
  uint32_t
  size() const {
    return size_;
  }

private:

  std::string const name_;
  std::string const suint_name_;
  std::string const duint_name_;
  uint32_t    const mantissa_size_;
  uint32_t    const exponent_size_;
  int32_t     const exponent_min_;
  uint32_t    const size_;
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
    P2P_    {pow2(fp_type_.mantissa_size())} {
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
      "  exponent_size  = " << fp_type_.exponent_size() << ",\n"
      "  mantissa_size  = " << fp_type_.mantissa_size() << ",\n"
      "  large_exponent = 10,\n" // LOG5_POW2(mantissa_size + 2)"
      "  word_size      = " << fp_type_.size() << ",\n"
      "  exponent_min   = " << fp_type_.exponent_min()   << "\n"
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

    auto const E2_max = fp_type_.exponent_min() +
      int32_t(uint32_t{1} << fp_type_.exponent_size()) - 2;

    for (int32_t e2 = fp_type_.exponent_min(); e2 < E2_max; ++e2) {

      auto const f          = log10_pow2(e2);
      auto const e          = e2 - f;

      auto const alpha      = f >= 0 ? pow2( e) : pow5(-f);
      auto const delta      = f >= 0 ? pow5( f) : pow2(-e);

      auto const start_at_1 = e2 == fp_type_.exponent_min();
      auto const maximum    = get_maximum(alpha, delta, start_at_1);
      auto const fast_eaf   = get_fast_eaf(alpha, delta, maximum, fixed_k);

      std::cerr <<
        e2                   << '\t' <<
        f                    << '\t' <<
        alpha                << '\t' <<
        delta                << '\t' <<
        numerator(maximum)   << '\t' <<
        denominator(maximum) << '\t' <<
        fast_eaf.U           << '\t' <<
        fast_eaf.k           << '\n';

      auto const upper = static_cast<uint32_t>(fast_eaf.U >> 32);
      auto const lower = static_cast<uint32_t>(fast_eaf.U);
      auto const shift = fast_eaf.k;

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
  get_maximum(integer_t alpha, integer_t const& delta, integer_t const& a,
    integer_t const& b) {

    auto const b_minus_1 = b - 1;

    auto const phi_0     = rational_t{b_minus_1,
      delta - alpha * b_minus_1 % delta};

    if (a == b_minus_1)
      return phi_0;

    if (alpha >= delta)
      alpha = alpha % delta;

    if (alpha == 0)
      return rational_t{b_minus_1, delta};

    auto const a1 = alpha * a / delta + 1;
    auto const b1 = alpha * b_minus_1 / delta + 1;

    if (a1 == b1)
      return phi_0;

    auto const& delta1  = alpha;
    auto const  alpha1  = delta1 - delta % delta1;
    auto const  phi_1   = get_maximum(alpha1, delta1, a1, b1);

    auto const  maximum = rational_t{
      delta * numerator(phi_1) - denominator(phi_1),
      delta1 * denominator(phi_1)};

    return std::max(maximum, phi_0);
  }

  rational_t
  get_maximum(integer_t alpha, integer_t const& delta, bool start_at_1 = false)
    const {

    alpha               %= delta;

    auto const a        = start_at_1 ? integer_t{1} : 2 * P2P_;
    auto const b        = 4 * P2P_;
    auto const maximum1 = get_maximum(alpha, delta, a, b);

    auto const m2       = 20 * P2P_;
    auto const r2       = m2 * alpha % delta;
    auto const maximum2 = rational_t{m2, delta - r2};

    return std::max(maximum1, maximum2);
  }

  fast_eaf_t
  get_fast_eaf(integer_t const& numerator, integer_t const& denominator,
    rational_t const& maximum, uint32_t /*fixed_k*/ = 0) const {

    auto k    = uint32_t{0};
    auto pow2 = integer_t{1}; // 2^k
    auto q    = integer_t{numerator / denominator};
    auto r    = integer_t{numerator % denominator};

    while (k <= 2 * fp_type_.size()) {

      if (maximum < rational_t{pow2, denominator - r})
        return { q + 1, k };

      k    += 1;
      pow2 *= 2;
      q    *= 2;
      r    *= 2;
      while (r >= denominator) {
        q += 1;
        r -= denominator;
      }
    }

    return { 0, 0 }; // Failed.
  }

  fp_type_t fp_type_;
  integer_t  P2P_;
};

int main() {

  auto float_config = fp_type_t{
    /* name          */ "float",
    /* suint_name    */ "uint32_t",
    /* suint_name    */ "uint64_t",
    /* mantissa_size */ 23,
    /* exponent_size */ 8,
    /* exponent_min  */ -149
  };
  auto generator = generator_t{float_config};
  auto stream    = std::ofstream{"./include/table32.h"};

//  auto double_config   = fp_type_t{
//    /* name          */ "double",
//    /* suint_name    */ "uint64_t",
//    /* suint_name    */ "__uint128_t",
//    /* mantissa_size */ 52,
//    /* exponent_size */ 11,
//    /* exponent_min  */ -1074
//  };
//  auto generator = generator_t{double_config};
//  auto stream    = std::ofstream{"./include/table64.h"};

  generator.generate(stream);
}
