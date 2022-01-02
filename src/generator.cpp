#include "common.h"
#include "math.hpp"

#include <climits>
#include <cstdint>
#include <cstdio>
#include <exception>
#include <iomanip>
#include <ios>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

/**
 * \brief Exception thrown by the generator.
 */
struct amaru_exception : std::range_error {
  using std::range_error::range_error;
};

/**
 * \brief Meta information about a floating point number type.
 *
 * Surprisingly, the name of the type is not necessary because Amaru doesn't
 * work directly with values of this type. Instead, they are decoded elsewhere
 * into Amaru's representation of floating point numbers which is a struct
 * containing sign, exponent and mantissa fields.
 *
 * Amaru needs, however, to know two unsigned integer types:
 *
 *    suint : used for mantissa storage and calculations.
 *    duint : used for even bigger calculations. It must be, at least, double
 *            the size of suint.
 */
struct info_t {

  /**
   * \brief Constructor.
   *
   * \param id              An identifier for the floating point number type
   *                        (e.g., "ieee32" or "ieee64"). This is used in C/C++
   *                        identifiers and, accordingly, is restricted to a
   *                        corresponding set of characters. In particular, it
   *                        contains no spaces - "long double" is forbidden.
   * \param suint           C/C++ name of suint (e.g., "uint32_t" or
   *                        "uint64_t"). It might contain spaces -- "unsigned
   *                        long" is allowed.
   * \param duint           C/C++ name of duint (e.g., "uint64_t" or
   *                        "uint128_t").
   * \param ssize           The size of suint in bits.
   * \param exponent_size   Size of exponent in bits.
   * \param exponent_min    Minimum exponent.
   * \param exponent_max    Maximum exponent.
   * \param mantissa_size   Size of mantissa in bits.
   */
  info_t(std::string id, std::string suint, std::string duint, uint32_t ssize,
    uint32_t exponent_size, int32_t exponent_min, int32_t exponent_max,
    uint32_t mantissa_size) :
    id_                 {std::move(id)                       },
    suint_              {std::move(suint)                    },
    duint_              {std::move(duint)                    },
    rep_                {id_ + "_t"                          },
    ssize_              {ssize                               },
    exponent_size_      {exponent_size                       },
    exponent_min_       {exponent_min                        },
    exponent_max_       {exponent_max                        },
    exponent_critical_  {log5_pow2(mantissa_size + 2)        },
    mantissa_size_      {mantissa_size                       },
    normal_mantissa_min_{AMARU_POW2(integer_t, mantissa_size)},
    normal_mantissa_max_{2 * normal_mantissa_min_            } {
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
   * \brief Returns the size in bits of suint.
   */
  uint32_t const& ssize() const {
    return ssize_;
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
   * \brief Returns the maximal exponent allowed in the implementation.
   */
  int32_t const& exponent_max() const {
    return exponent_max_;
  }

  /**
   * \brief Returns the critical exponent.
   */
  int32_t const& exponent_critical() const {
    return exponent_critical_;
  }

  /**
   * \brief Returns the normal minimal mantissa: pow2(mantissa_size()).
   *
   * This is the minimal mantissa for normal numbers. Sub-normal numbers have
   * smaller mantissas.
   */
  integer_t const& normal_mantissa_min() const {
    return normal_mantissa_min_;
  }

  /**
   * \brief Returns the normal maximal mantissa: 2 * normal_mantissa_min().
   *
   * This is the maximal mantissa for normal numbers. Sub-normal numbers have
   * smaller mantissas.
   */
  integer_t const& normal_mantissa_max() const {
    return normal_mantissa_max_;
  }

private:

  std::string const id_;
  std::string const suint_;
  std::string const duint_;
  std::string const rep_;
  uint32_t    const ssize_;
  uint32_t    const exponent_size_;
  int32_t     const exponent_min_;
  int32_t     const exponent_max_;
  int32_t     const exponent_critical_;
  uint32_t    const mantissa_size_;
  integer_t   const normal_mantissa_min_;
  integer_t   const normal_mantissa_max_;
};

/**
 * \brief Configuration of Amaru's implementation.
 */
struct config_t {

  /**
   * \brief Constructor.
   *
   * \param use_same_shift  Tells if Amaru should use the same shift for all
   *                        exponents. It saves memory and makes Amaru faster.
   *                        Unfortunately, such shift might might be too large
   *                        for practical use, in which case, the generator
   *                        throws.
   *
   * \param use_compact_tbl Tells if the multipliers table should have an entry
   *                        per decimal (compact) or per binary exponent
   *                        (complete). The compact form requires a few more
   *                        instructions at runtime. However, the size of the
   *                        compact table asymptotically goes to log_10(2) (or
   *                        30%) of the non compact one. In addition to saving
   *                        memory this might reduce cache misses, possibly,
   *                        boosting performance.
   *
   * \param identify_special_cases  Tells if Amaru should identify special cases
   *                        (e.g., when binary exponent in {0, 1}) and treat
   *                        them differently using a more direct and,
   *                        presumably, more efficient method.
   */
  config_t(bool use_same_shift, bool use_compact_tbl,
      bool identify_special_cases) :
    use_same_shift_        {use_same_shift        },
    use_compact_tbl_       {use_compact_tbl       },
    identify_special_cases_{identify_special_cases} {
  }

  /**
   * \brief Returns if Amaru should use the same shift for all exponents.
   */
  bool use_same_shift() const {
    return use_same_shift_;
  }

  /**
   * \brief Returns if Amaru should use a compact table of multipliers.
   */
  bool use_compact_tbl() const {
    return use_compact_tbl_;
  }

  /**
   * \brief Returns if Amaru should indentify special cases and treat them
   * differently.
   */
  bool identify_special_cases() const {
    return identify_special_cases_;
  }

private:
  bool use_same_shift_;
  bool use_compact_tbl_;
  bool identify_special_cases_;
};

/**
 * \brief Generator of Amaru's implementation for a given floating point number
 * type.
 */
struct generator_t {

  /**
   * \brief Constructor.
   *
   * \param info            The meta information of the floating point number
   *                        type whose implementation is to be generated.
   * \param config          Configures implementation details.
   */
  generator_t(info_t info, config_t config) :
    info_   {std::move(info)              },
    config_ {std::move(config)            },
    p2ssize_{integer_t{1} << info_.ssize()} {
  }

  /**
   * \brief Generates the implementation.
   *
   * \brief dot_h The output stream to receive the ".h" file content.
   * \brief dot_c The output stream to receive the ".c" file content.
   */
  void generate(std::ostream& dot_h, std::ostream& dot_c) const {

    std::cout << "Generation started.\n";

    // Overflow check 1:
    if (2 * info_.normal_mantissa_max() + 1 >= p2ssize_)
      throw amaru_exception("suint_t is not large enough for calculations to "
        "not overflow.");

    // Overflow check 2:
    if (20 * info_.normal_mantissa_min() >= p2ssize_)
      throw amaru_exception("suint_t is not large enough for calculations to "
        "not overflow.");

    std::cout << "  Generating \".h\".\n";
    header(dot_h);

    std::cout << "  Generating \".c\".\n";
    source(dot_c);

    std::cout << "Generation finished.\n";
  }

private:

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
   * \brief Stores alpha, delta (usually pow2(e) and pow2(f)) and the maximum of
   *     m / (delta - alpha * m % delta)
   * for m in the set of mantissas.
   */
  struct alpha_delta_maximum {
    integer_t  alpha;
    integer_t  delta;
    rational_t maximum;
  };

  /**
   * \brief The objective function of the primary maximisation problem:
   *
   *     phi(m) := m / (delta - alpha * m % delta).
   */
  static rational_t phi(integer_t const& alpha, integer_t const& delta,
    integer_t const& m) {
    return {m, delta - alpha * m % delta};
  }

  /**
   * \brief The objective function of the secondary maximisation problem:
   *
   *     phi'(m') := m' / (1 + (alpha' * m' - 1) % delta').
   */
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

    dot_h << "#pragma once\n\n";

    common_initial(dot_h);

    dot_h <<
      info_.rep() << "\n"
      "to_amaru_dec_" << info_.id() << "(bool negative, int32_t exponent, " <<
      info_.suint() << " mantissa);\n";

    common_final(dot_h);
  }

  /**
   * \brief Streams out the source file.
   *
   * \brief dot_c The output stream to receive the ".c" file content.
   */
  void source(std::ostream& dot_c) const {
    common_initial(dot_c);

    auto const maxima = get_maxima();
    std::vector<fast_eaf_t> fast_eafs;
    fast_eafs.reserve(maxima.size());

    auto shift = uint32_t{0};

    // Calculates minimal fast EAFs (i.e., those with minimal shift).
    for (auto const& x : maxima) {
      fast_eafs.emplace_back(get_fast_eaf(x));
      auto const s = fast_eafs.back().k;
      if (s > shift)
        shift = s;
    }

    // Replace minimal fast EAFs to use the same shift.
    if (config_.use_same_shift()) {

      auto const p2shift = integer_t{1} << shift;

      for (uint32_t i = 0; i < maxima.size(); ++i) {

        auto const& x = maxima[i];

        integer_t q, r;
        divide_qr(x.alpha << shift, x.delta, q, r);

        if (x.maximum >= rational_t{p2shift, x.delta - r})
          throw amaru_exception{"Unable to use same shift."};

        fast_eafs[i] = fast_eaf_t{q + 1, shift};
      }
    }

    auto const ssize   = info_.ssize();
    auto const p2ssize = integer_t{1} << ssize;

    // Check if all upper parts of the multiplier are zero.
    auto all_upper_parts_are_zero = true;

    for (auto const& fast_eaf : fast_eafs)
      if (fast_eaf.U >= p2ssize) {
        all_upper_parts_are_zero = false;
        break;
      }

    dot_c <<
      "typedef " << info_.suint() << " suint_t;\n"
      "typedef " << info_.duint() << " duint_t;\n"
      "typedef " << info_.rep()   << " rep_t;\n"
      "\n"
      "static uint32_t const mantissa_size         = " <<
        info_.mantissa_size() << ";\n"
      "static int32_t  const bin_exponent_min      = " <<
        info_.exponent_min() << ";\n";

     if (config_.use_compact_tbl())
       dot_c << "static int32_t  const dec_exponent_min      = " <<
         log10_pow2(info_.exponent_min()) << ";\n";

     dot_c << "static int32_t  const bin_exponent_critical = " <<
        info_.exponent_critical() << ";\n"
      "static suint_t  const normal_mantissa_min   = " <<
        info_.normal_mantissa_min() << ";\n"
      "\n";

    if (all_upper_parts_are_zero)
      dot_c << "#define AMARU_UPPER_IS_ZERO\n\n";

    if (config_.use_compact_tbl())
      dot_c << "#define AMARU_USE_COMPACT_TBL\n\n";

    if (config_.use_same_shift())
      dot_c << "#define AMARU_SHIFT " << shift << "\n\n";

    if (config_.identify_special_cases())
      dot_c << "#define AMARU_IDENTIFY_SPECIAL_CASES\n\n";

    dot_c << "static struct {\n";

    if (!all_upper_parts_are_zero)
      dot_c << "  suint_t  const upper;\n";

    dot_c << "  suint_t  const lower;\n";

    if (!config_.use_same_shift())
      dot_c << "  uint32_t const shift;\n";

    dot_c << "} const multipliers [] = {\n";

    auto const nibbles = ssize / 4;

    auto e2      = info_.exponent_min();
    auto e2_or_f = config_.use_compact_tbl() ? log10_pow2(e2) : e2;

    for (auto const& fast_eaf : fast_eafs) {

      integer_t upper, lower;
      divide_qr(fast_eaf.U, p2ssize, upper, lower);

      if (upper >= p2ssize)
        throw amaru_exception{"Multiplier is out of range."};

      dot_c << "  { ";

      if (!all_upper_parts_are_zero) {
        dot_c << "0x" << std::hex << std::setw(nibbles) << std::setfill('0') <<
            upper << ", ";
      }

      dot_c << "0x" << std::hex << std::setw(nibbles) << std::setfill('0') <<
        lower << std::dec;

      if (!config_.use_same_shift()) {
        auto const shift = fast_eaf.k;
        dot_c << ", " << shift;
      }
      dot_c << " }, // " << e2_or_f << "\n";
      ++e2_or_f;
    }

    dot_c <<
      "};\n"
      "\n"
      "#define TO_AMARU_DEC to_amaru_dec_" << info_.id() << "\n"
      "#include \"src/amaru.h\"\n"
      "#undef AMARU\n";
    common_final(dot_c);
  }

  /**
   * \brief Gets the maxima of all primary problems. (See get_maximum_primary.)
   *
   * It returns a vector v of size info_.exponent_max() - info_.exponent_min() +
   * 1 such that v[i] contains the maximum of the primary problem corresponding
   * to exponent = info_.exponent_min() + i.
   *
   * \returns The vector v.
   */
  std::vector<alpha_delta_maximum> get_maxima() const {

    std::vector<alpha_delta_maximum> maxima;
    maxima.reserve(info_.exponent_max() - info_.exponent_min() + 1);

    auto f_done = log10_pow2(info_.exponent_min()) - 1;

    for (auto e2 = info_.exponent_min(); e2 <= info_.exponent_max(); ++e2) {

      auto const f = log10_pow2(e2);

      if (config_.use_compact_tbl() && f == f_done)
        continue;

      auto const e = (config_.use_compact_tbl() ? log2_pow10(f) : e2) - f;

      alpha_delta_maximum x;
      x.alpha   = f >= 0 ? pow2(e) : pow5(-f);
      x.delta   = f >= 0 ? pow5(f) : pow2(-e);
      x.maximum = get_maximum(x.alpha, x.delta, e2 == info_.exponent_min());

      maxima.emplace_back(std::move(x));

      f_done = f;
    }
    return maxima;
  }

  /**
   * \brief Given alpha, delta, a and b, this function calculates the maximiser
   * of phi(m) over [a, b[.
   *
   * \pre 0 <= alpha && alpha < delta && a < b.
   */
  static rational_t get_maximum_primary(integer_t const& alpha,
    integer_t const& delta, integer_t const& a, integer_t const& b) {

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

  /**
   * \brief Given alpha', delta', a' and b', this function calculates the
   * maximiser of phi'(m') over [a', b'[.
   *
   * \pre 0 < alpha' && 0 < delta' && 1 <= a' && a' < b'.
   */
  static rational_t get_maximum_secondary(integer_t const& alpha_p,
    integer_t const& delta_p, integer_t const& a_p, integer_t const& b_p) {

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

  /**
   * \brief Given alpha and delta, this function calculates the maximiser of
   * phi(m) over the relevant set of mantissas.
   *
   * \pre 0 <= alpha && 0 < delta.
   */
  rational_t get_maximum(integer_t alpha, integer_t const& delta,
    bool start_at_1 = false) const {

    alpha               %= delta;

    auto const a        = start_at_1 ?
      integer_t{1} : 2 * info_.normal_mantissa_min();
    auto const b        = 2 * info_.normal_mantissa_max() *
      (config_.use_compact_tbl() ? 8 : 1);

    auto const maximum1 = get_maximum_primary(alpha, delta, a, b);
    auto const maximum2 = phi(alpha, delta, 20 * info_.normal_mantissa_min());

    return std::max(maximum1, maximum2);
  }

  /**
   * \brief Get the EAF f(m) = alpha * m / delta which works on an interval of
   * relevant mantissas. This fast EAF is associated to maximisation of phi(m)
   * over the set of mantissas.
   *
   * \param x               The container of alpha, beta and the solution of
   *                        the primary maximisation problem.
   */
  fast_eaf_t get_fast_eaf(alpha_delta_maximum const& x) const {

    auto k    = uint32_t{0};
    auto pow2 = integer_t{1}; // 2^k

    integer_t q, r;
    divide_qr(x.alpha, x.delta, q, r);

    // It should return from inside the loop but let's put a bound.
    while (k < 3 * info_.ssize()) {

      if (x.maximum < rational_t{pow2, x.delta - r})
        return { q + 1, k };

      k    += 1;
      pow2 *= 2;
      q    *= 2;
      r    *= 2;
      while (r >= x.delta) {
        q += 1;
        r -= x.delta;
      }
    }

    throw amaru_exception{"Cannot find fast EAF."};
  }

  info_t    info_;
  config_t  config_;
  integer_t p2ssize_;
};

int main() {

  try {

    auto const config = config_t{
      /* use_same_shift         */ false,
      /* use_compact_tbl        */ false,
      /* identify_special_cases */ false
    };

    auto ieee32_info = info_t{
      /* id            */ "ieee32",
      /* suint         */ "uint32_t",
      /* duint         */ "uint64_t",
      /* ssize         */ 32,
      /* exponent_size */ 8,
      /* exponent_min  */ -149,
      /* exponent_max  */ 104,
      /* mantissa_size */ 23
    };
    auto generator_32 = generator_t{ieee32_info, config};
    auto dot_h_32     = std::ofstream{"../generated/ieee32.h"};
    auto dot_c_32     = std::ofstream{"../generated/ieee32.c"};
    generator_32.generate(dot_h_32, dot_c_32);

    auto ieee64_info   = info_t{
      /* id            */ "ieee64",
      /* suint         */ "uint64_t",
      /* duint         */ "__uint128_t",
      /* ssize         */ 64,
      /* exponent_size */ 11,
      /* exponent_min  */ -1074,
      /* exponent_max  */ 971,
      /* mantissa_size */ 52
    };
    auto generator_64 = generator_t{ieee64_info, config};
    auto dot_h_64     = std::ofstream{"../generated/ieee64.h"};
    auto dot_c_64     = std::ofstream{"../generated/ieee64.c"};
    generator_64.generate(dot_h_64, dot_c_64);
  }

  catch (amaru_exception const& e) {
    std::printf("Generation failed: %s.\n", e.what());
  }

  catch (std::exception const& e) {
    std::printf("std::exception thrown: %s.\n", e.what());
  }

  catch (...) {
    std::printf("Unknown exception thrown.\n");
  }
}
