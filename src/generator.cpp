// g++ -O3 -std=c++11 -I./include src/generator.cpp -o generator -Wall -Wextra

#include "common.h"

#include <boost/multiprecision/cpp_int.hpp>

#include <algorithm>
#include <climits>
#include <cstdint>
#include <cstdio>
#include <exception>
#include <iomanip>
#include <ios>
#include <iostream>
#include <fstream>
#include <string>

namespace {

using integer_t  = boost::multiprecision::cpp_int;
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
  uint32_t  k;
};

/**
 * \brief Returns 2^e.
 */
integer_t
pow2(uint32_t e) {
  return integer_t{1} << e;
}

/**
 * \brief Returns 5^e.
 */
integer_t
pow5(uint32_t e) {
  if (e == 0)
    return 1;
  auto const p1 = pow5(e / 2);
  return p1 * p1 * (e % 2 == 0 ? 1 : 5);
}

/**
 * \brief Returns the integer part of log_5(2^n).
 *
 * \pre n in [-78854, 78855[.
 */
static inline
int32_t
log5_pow2(int32_t n) {
  int64_t const log5_2 = 1849741732;
  return n >= 0 ? (int32_t) (log5_2 * n >> 32) : -log5_pow2(-n) - 1;
}

struct amaru_exception : std::range_error {
  using std::range_error::range_error;
};

/**
 * \brief Meta information about a floating point number type required by Amaru.
 *
 * Surprisingly, the name of the type is not necessary because Amaru doesn't
 * work directly with values of this type. Instead, they are decoded elsewhere
 * into Amaru's representation of floating point numbers which is a struct
 * containing sign, exponent and mantissa fields.
 *
 * However, Amaru needs to know two unsigned integer types:
 *
 *    suint : used for mantissa storage and calculations.
 *    duint : used for even bigger calculations. It must be, at least, double
 *            the size of suint.
 */
struct fp_info_t {

  /**
   * \brief Constructor.
   *
   * \param id              An identifier for the floating point number type
   *                        (e.g., "ieee32" or "ieee64". This is used in C/C++
   *                        identifiers and, accordingly, is restricted to the
   *                        valid set of characters used in identifiers. In
   *                        particular, it contains no spaces - "long double"
   *                        is forbidden.
   * \param suint           C/C++ name of suint (e.g., "uint32_t" or
   *                        "uint64_t"). It might contain spaces -- "unsigned
   *                        long" is allowed.
   * \param duint           C/C++ name of duint (e.g., "uint64_t" or
   *                        "uint128_t").
   * \param exponent_size   Size of exponent in bits.
   * \param mantissa_size   Size of mantissa in bits.
   * \param exponent_min    Minimum exponent. (As described in Amaru's
   *                        representation.)
   */
  fp_info_t(std::string id, std::string suint, std::string duint,
      uint32_t exponent_size, uint32_t mantissa_size, int32_t exponent_min) :
    id_                {std::move(id)                       },
    suint_             {std::move(suint)                    },
    duint_             {std::move(duint)                    },
    rep_               {id_ + "_t"                          },
    exponent_size_     {exponent_size                       },
    mantissa_size_     {mantissa_size                       },
    exponent_min_      {exponent_min                        },
    exponent_critical_ {log5_pow2(mantissa_size + 2)        },
    pow2_mantissa_size_{AMARU_POW2(integer_t, mantissa_size)} {
  }

  /**
   * \brief Returns the id.
   */
  std::string const& id() const {
    return id_;
  }

  /**
   * \brief Returns the C/C++ name of suint.
   */
  std::string const& suint() const {
    return suint_;
  }

  /**
   * \brief Returns the C/C++ name of duint.
   */
  std::string const& duint() const {
    return duint_;
  }

  /**
   * \brief Returns the C/C++ name of Amaru's representation type.
   */
  std::string const& rep() const {
    return rep_;
  }

  /**
   * \brief Returns size of exponent in bits.
   */
  uint32_t const& exponent_size() const {
    return exponent_size_;
  }

  /**
   * \brief Returns the size of mantissa in bits.
   */
  uint32_t const& mantissa_size() const {
    return mantissa_size_;
  }

  /**
   * \brief Returns the minimum exponent. (As described in Amaru's
   * representation.)
   */
  int32_t const& exponent_min() const {
    return exponent_min_;
  }

  /**
   * \brief Returns the critical exponent.
   */
  int32_t const& exponent_critical() const {
    return exponent_critical_;
  }

  /**
   * \brief Returns pow2(mantissa_size()).
   */
  integer_t const& pow2_mantissa_size() const {
    return pow2_mantissa_size_;
  }
private:

  std::string const id_;
  std::string const suint_;
  std::string const duint_;
  std::string const rep_;
  uint32_t    const exponent_size_;
  uint32_t    const mantissa_size_;
  int32_t     const exponent_min_;
  int32_t     const exponent_critical_;
  integer_t   const pow2_mantissa_size_;
};

/**
 * \brief Amaru's table generator.
 */
struct generator_t {

  /**
   * \brief Constructor.
   *
   * \param fp_info_t The meta information of the floating point number type
   *                whose table shall be generated.
   */
  generator_t(fp_info_t fp_info) :
    info_{std::move(fp_info)} {
  }

  /**
   * \brief Generates Amaru's implementation for fp.
   *
   * \brief dot_h The output stream to receive the ".h" file content.
   * \brief dot_c The output stream to receive the ".c" file content.
   */
  void generate(std::ostream& dot_h, std::ostream& dot_c) const {

    std::cout << "Generation started.\n";

    std::cout << "  Generating \".h\".\n";
    header(dot_h);

    std::cout << "  Generating \".c\".\n";
    source(dot_c);

    std::cout << "Generation finished.\n";
  }

private:

  static rational_t phi(integer_t const& alpha, integer_t const& delta,
    integer_t const& m) {
    return {m, delta - alpha * m % delta};
  }

  static rational_t phi_p(integer_t const& alpha, integer_t const& delta,
    integer_t const& m) {
    return {m, 1 + (alpha * m - 1) % delta};
  }

  /**
   * \brief Streams out the common initial part of header and source files.
   *
   * \brief stream The output stream to receive the content.
   */
  void common_initial(std::ostream& stream) const {
    stream <<
      "// This file is auto-generated. DO NOT EDIT IT.\n"
      "\n"
      "#include <stdint.h>\n"
      "\n"
      "#ifndef __cplusplus\n"
      "#include <stdbool.h>\n"
      "#endif\n"
      "\n"
      "#ifdef __cplusplus\n"
      "extern \"C\" {\n"
      "#endif\n"
      "\n"
      "typedef struct {\n"
      "  bool    negative;\n"
      "  int32_t exponent;\n"
      "  " << info_.suint() << " mantissa;\n"
      "} " << info_.rep() << ";\n"
      "\n";
  }

  /**
   * \brief Streams out the common final part of header and source files.
   *
   * \brief stream The output stream to receive the content.
   */
  void common_final(std::ostream& stream) const {
    stream <<
      "\n"
      "#ifdef __cplusplus\n"
      "}\n"
      "#endif\n";
  }

  /**
   * \brief Streams out the header file.
   *
   * \brief dot_h The output stream to receive the ".h" file content.
   */
  void header(std::ostream& dot_h) const {
    dot_h << "#pragma once\n"
      "\n";
    common_initial(dot_h);
    dot_h <<
      info_.rep() << " to_amaru_dec_" << info_.id() << "(" << info_.rep() << " amaru_bin);\n";
    common_final(dot_h);
  }

  /**
   * \brief Streams out the source file.
   *
   * \brief dot_c The output stream to receive the ".c" file content.
   */
  void source(std::ostream& dot_c) const {
    common_initial(dot_c);

    dot_c <<
      "typedef " << info_.suint() << " suint_t;\n"
      "typedef " << info_.duint() << " duint_t;\n"
      "typedef " << info_.rep()   << " rep_t;\n"
      "\n"
      "static uint32_t const mantissa_size     = " << info_.mantissa_size()
      << ";\n"
      "static int32_t  const exponent_min      = " << info_.exponent_min()
      << ";\n"
      "static int32_t  const exponent_critical = " << info_.exponent_critical()
      << ";\n"
      "static duint_t  const mantissa_critical = " << info_.pow2_mantissa_size()
      << ";\n"
      "\n"
      "static struct {\n"
      "  suint_t const upper;\n"
      "  suint_t const lower;\n"
      "  uint32_t const shift;\n"
      "} scalers[] = {\n";

    auto const e2_max = info_.exponent_min() +
      int32_t(uint32_t{1} << info_.exponent_size()) - 2;

    auto const size       = 1 + info_.exponent_size() + info_.mantissa_size();
    auto const p2size     = integer_t{1} << size;
    auto const nibbles    = size / 4;

    for (auto e2 = info_.exponent_min(); e2 < e2_max; ++e2) {

      auto const f          = log10_pow2(e2);
      auto const e          = e2 - f;

      auto const alpha      = f >= 0 ? pow2( e) : pow5(-f);
      auto const delta      = f >= 0 ? pow5( f) : pow2(-e);

      auto const start_at_1 = e2 == info_.exponent_min();
      auto const maximum    = get_maximum(alpha, delta, start_at_1);
      auto const fast_eaf   = get_fast_eaf(alpha, delta, maximum, fixed_k);

      integer_t upper, lower;
      divide_qr(fast_eaf.U, p2size, upper, lower);

      if (upper > p2size)
        throw amaru_exception{"Multiplier is out of range."};

      auto const shift = fast_eaf.k;

      dot_c << "  { " <<
        "0x"     << std::hex << std::setw(nibbles) << std::setfill('0') <<
        upper    << ", " <<
        "0x"     << std::hex << std::setw(nibbles) << std::setfill('0') <<
        lower    << ", " <<
        std::dec <<
        shift    << " }, // " <<
        e2       << "\n";
    }

    dot_c <<
      "};\n"
      "\n"
      "#define TO_AMARU_DEC to_amaru_dec_" << info_.id() << "\n"
      "#include \"src/amaru.h\"\n"
      "#undef AMARU\n";
    common_final(dot_c);
  }

  rational_t static
  get_maximum_primary(integer_t const& alpha, integer_t const& delta,
    integer_t const& a, integer_t const& b) {

    auto const b_minus_1 = b - 1;
    auto const maximum1  = phi(alpha, delta, b_minus_1);

    if (alpha == 0 || a == b_minus_1)
      return maximum1;

    auto const a_p = alpha * a / delta + 1;
    auto const b_p = alpha * b_minus_1 / delta + 1;

    if (a_p == b_p)
      return maximum1;

    auto const  alpha_p = delta % alpha;
    auto const& delta_p = alpha;
    auto const  other   = get_maximum_secondary(alpha_p, delta_p, a_p, b_p);

    auto const  maximum2 = rational_t{
      delta * numerator(other) - denominator(other),
      alpha * denominator(other)};

    return std::max(maximum1, maximum2);
  }

  rational_t static
  get_maximum_secondary(integer_t const& alpha_p, integer_t const& delta_p,
    integer_t const& a_p, integer_t const& b_p) {

    if (alpha_p == 0)
      return b_p - 1;

    auto const maximum1 = phi_p(alpha_p, delta_p, a_p);

    if (a_p == b_p - 1)
      return maximum1;

    auto const a1 = (alpha_p * a_p - 1) / delta_p + 1;
    auto const b1 = (alpha_p * (b_p - 1) - 1) / delta_p + 1;

    if (a1 == b1)
      return maximum1;

    auto const  alpha1 = delta_p % alpha_p;
    auto const& delta1 = alpha_p;
    auto const  other  = get_maximum_primary(alpha1, delta1, a1, b1);

    auto const  maximum2 = rational_t{
      delta_p * numerator(other) + denominator(other),
      alpha_p * denominator(other)};

    return std::max(maximum1, maximum2);
  }

  rational_t
  get_maximum(integer_t alpha, integer_t const& delta, bool start_at_1 = false)
    const {

    alpha               %= delta;

    auto const a        = start_at_1 ?
      integer_t{1} : 2 * info_.pow2_mantissa_size();
    auto const b        = 4 * info_.pow2_mantissa_size();

    auto const maximum1 = get_maximum_primary(alpha, delta, a, b);
    auto const maximum2 = phi(alpha, delta, 20 * info_.pow2_mantissa_size());

    return std::max(maximum1, maximum2);
  }

  fast_eaf_t
  get_fast_eaf(integer_t const& numerator, integer_t const& denominator,
    rational_t const& maximum, uint32_t /*fixed_k*/ = 0) const {

    auto k    = uint32_t{0};
    auto pow2 = integer_t{1}; // 2^k

    integer_t q, r;
    divide_qr(numerator, denominator, q, r);

    // It should return from inside the loop but let's put a bound.
    while (k < 3 * 64) {

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

    throw amaru_exception{"Cannot find fast EAF."};
  }

  fp_info_t info_;
};

} // namespace <anonymous>

int main() {

  try {

    #if 1
      auto float_config = fp_info_t {
        /* name          */ "ieee32",
        /* suint         */ "uint32_t",
        /* duint         */ "uint64_t",
        /* exponent_size */ 8,
        /* mantissa_size */ 23,
        /* exponent_min  */ -149
      };
      auto generator = generator_t{float_config};
      auto dot_h = std::ofstream{"./generated/ieee32.h"};
      auto dot_c = std::ofstream{"./generated/ieee32.c"};
  #else
    auto double_config   = fp_info_t{
      /* name          */ "ieee64",
      /* suint         */ "uint64_t",
      /* duint         */ "__uint128_t",
      /* exponent_size */ 11,
      /* mantissa_size */ 52,
      /* exponent_min  */ -1074
    };
    auto generator = generator_t{double_config};
    auto dot_h = std::ofstream{"./generated/ieee64.h"};
    auto dot_c = std::ofstream{"./generated/ieee64.c"};
  #endif

    generator.generate(dot_h, dot_c);
  }

  catch (std::exception const& e) {
    printf("std::exception thrown: %s.\n", e.what());
  }

  catch (...) {
    printf("Unknown exception thrown.\n");
  }
}
