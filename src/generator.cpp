#include "../include/common.h"

#include <boost/multiprecision/cpp_int.hpp>

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

using integer_t  = boost::multiprecision::cpp_int;
using rational_t = boost::multiprecision::cpp_rational;

/**
 * \brief Returns 2^n.
 */
integer_t pow2(uint32_t n) {
  return integer_t{1} << n;
}

/**
 * \brief Returns 5^n.
 */
integer_t pow5(uint32_t n) {
  if (n == 0)
    return 1;
  auto const p1 = pow5(n / 2);
  return p1 * p1 * (n % 2 == 0 ? 1 : 5);
}

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
   * \param id                An identifier for the floating point number type
   *                          (e.g., "ieee32" or "ieee64"). This is used in
   *                          C/C++ identifiers and, accordingly, is restricted
   *                          to a corresponding set of characters. In
   *                          particular, it must not contain spaces --
   *                          "long double" is forbidden).
   * \param suint             C/C++ name of suint (e.g., "uint32_t" or
   *                          "uint64_t"). It might contain spaces -- "unsigned
   *                          long" is allowed.
   * \param duint             C/C++ name of duint (e.g., "uint64_t" or
   *                          "uint128_t").
   * \param ssize             The size of suint in bits.
   * \param exponent_size     Size of exponent in bits.
   * \param bin_exponent_min  Minimum exponent in binary.
   * \param bin exponent_max  Maximum exponent in binary.
   * \param mantissa_size     Size of mantissa in bits.
   */
  info_t(std::string id, std::string suint, std::string duint, uint32_t ssize,
    uint32_t exponent_size, int32_t bin_exponent_min, int32_t bin_exponent_max,
    uint32_t mantissa_size) :
    id_                 {std::move(id)                       },
    function_           {"amaru_bin_to_dec_" + id_           },
    suint_              {std::move(suint)                    },
    duint_              {std::move(duint)                    },
    rep_                {id_ + "_t"                          },
    ssize_              {ssize                               },
    exponent_size_      {exponent_size                       },
    bin_exponent_min_   {bin_exponent_min                    },
    bin_exponent_max_   {bin_exponent_max                    },
    dec_exponent_min_   {log10_pow2(bin_exponent_min)        },
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
   * \brief Returns the name of Amaru's function.
   */
  std::string const& function() const {
    return function_;
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
   * \brief Returns the binary minimum exponent. (As described in Amaru's
   * representation.)
   */
  int32_t const& bin_exponent_min() const {
    return bin_exponent_min_;
  }

  /**
   * \brief Returns the binary maximal exponent.
   */
  int32_t const& bin_exponent_max() const {
    return bin_exponent_max_;
  }

  /**
   * \brief Returns the decimal minimum exponent.
   */
  int32_t const& dec_exponent_min() const {
    return dec_exponent_min_;
  }

  /**
   * \brief Returns the normal (inclusive) minimal mantissa.
   *
   * Mantissas for normal floating point numbers are elements of the interval
   * [normal_mantissa_min(), normal_mantissa_max()[.
   */
  integer_t const& normal_mantissa_min() const {
    return normal_mantissa_min_;
  }

  /**
   * \brief Returns the normal (exclusive) maximal mantissa.
   *
   * Mantissas for normal floating point numbers are elements of the interval
   * [normal_mantissa_min(), normal_mantissa_max()[.
   */
  integer_t const& normal_mantissa_max() const {
    return normal_mantissa_max_;
  }

private:

  std::string const id_;
  std::string const function_;
  std::string const suint_;
  std::string const duint_;
  std::string const rep_;
  uint32_t    const ssize_;
  uint32_t    const exponent_size_;
  int32_t     const bin_exponent_min_;
  int32_t     const bin_exponent_max_;
  int32_t     const dec_exponent_min_;
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
   * \param use_compact_tbl Tells if the multipliers table should have an entry
   *                        per decimal (compact) or per binary exponent
   *                        (complete). The compact form requires a few more
   *                        instructions at runtime. However, the size of the
   *                        compact table asymptotically goes to log_10(2) (or
   *                        30%) of the non compact one. In addition to saving
   *                        memory this might reduce cache misses, possibly,
   *                        boosting performance.
   *
   * \param directory       Directory where generated files are created.
   */
  config_t(bool use_compact_tbl, std::string directory) :
    use_compact_tbl_{use_compact_tbl     },
    directory_      {std::move(directory)} {
  }

  /**
   * \brief Returns if Amaru should use a compact table of multipliers.
   */
  bool use_compact_tbl() const {
    return use_compact_tbl_;
  }

  /**
   * \brief Returns the directory where generated files are created.
   */
  std::string const& directory() const {
    return directory_;
  }

private:
  bool        use_compact_tbl_;
  std::string directory_;
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
      auto const prefix = config_.directory() + "/" + info_.id();
      dot_h_ = prefix + ".h";
      dot_c_ = prefix + ".c";
  }

  /**
   * \brief Returns the name of the .h file.
   */
  std::string const& dot_h() const {
    return dot_h_;
  }

  /**
   * \brief Returns the name of the .c file.
   */
  std::string const& dot_c() const {
    return dot_c_;
  }

  /**
   * \brief Generates the declaration and implementation.
   */
  void generate() const {

    auto dot_h_stream = std::ofstream{dot_h()};
    auto dot_c_stream = std::ofstream{dot_c()};

    std::cout << "Generation started.\n";

    // Overflow check 1:
    if (2 * info_.normal_mantissa_max() + 1 >= p2ssize_)
      throw amaru_exception("suint_t is not large enough for calculations to "
        "not overflow.");

    // Overflow check 2:
    if (20 * info_.normal_mantissa_min() >= p2ssize_)
      throw amaru_exception("suint_t is not large enough for calculations to "
        "not overflow.");

    std::cout << "  Generating \"" << dot_h() << "\".\n";
    generate_dot_h(dot_h_stream);

    std::cout << "  Generating \"" << dot_c() << "\".\n";
    generate_dot_c(dot_c_stream);

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
   * \brief Streams out the .h file.
   *
   * \param stream Output stream to receive the content.
   */
  void generate_dot_h(std::ostream& stream) const {

    stream <<
      "// This file is auto-generated. DO NOT EDIT IT.\n"
      "\n"
      "#pragma once\n"
      "\n"
      "#include <stdbool.h>\n"
      "#include <stdint.h>\n"
      "\n"
      "#ifdef __cplusplus\n"
      "extern \"C\" {\n"
      "#endif\n"
      "\n"
      "typedef struct {\n"
      "  bool negative;\n"
      "  int32_t exponent;\n"
      "  " << info_.suint() << " mantissa;\n"
      "} " << info_.rep() << ";\n"
      "\n" <<
        info_.rep() << ' ' << info_.function() << "(bool negative, "
        "int32_t exponent, " << info_.suint() << " mantissa);\n"
      "\n" <<
      "#ifdef __cplusplus\n"
      "}\n"
      "#endif\n";
  }

  /**
   * \brief Streams out the .c file.
   *
   * \param stream Output stream to receive the content.
   */
  void generate_dot_c(std::ostream& stream) const {

    auto const ssize = info_.ssize();

    stream << "// This file is auto-generated. DO NOT EDIT IT.\n"
      "\n" <<
      "#include \"" << dot_h() << "\"\n"
      "\n"
      "#ifdef __cplusplus\n"
      "extern \"C\" {\n"
      "#endif\n"
      "\n";

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

    // Optimal shift is 2 * size since it prevents multipliy_and_shift to
    // deal with partial limbs. In addition to subtract 1 to compensate the
    // increment adjustment made when the shift is output.
    if (config_.use_compact_tbl())
      shift = 2 * ssize - 1;

    // Replace minimal fast EAFs to use the same shift.

    auto const p2shift = integer_t{1} << shift;

    for (uint32_t i = 0; i < maxima.size(); ++i) {

      auto const& x = maxima[i];

      integer_t q, r;
      divide_qr(x.alpha << shift, x.delta, q, r);

      if (x.maximum >= rational_t{p2shift, x.delta - r})
        throw amaru_exception{"Unable to use same shift."};

      fast_eafs[i] = fast_eaf_t{q + 1, shift};
    }

    auto const p2ssize = integer_t{1} << ssize;

    stream <<
      "typedef " << info_.suint() << " suint_t;\n"
      "typedef " << info_.duint() << " duint_t;\n"
      "typedef " << info_.rep()   << " rep_t;\n"
      "\n"
      "enum {\n"
      "  ssize            = " << info_.ssize()            << ",\n"
      "  mantissa_size    = " << info_.mantissa_size()    << ",\n"
      "  bin_exponent_min = " << info_.bin_exponent_min() << ",\n"
      "  dec_exponent_min = " << info_.dec_exponent_min() << ",\n"
      // Instead of Amaru dividing multipliy_and_shift(m_a, upper, lower) by 2
      // we increment the shift here so this has the same effect.
      "  shift            = " << shift + 1                << "\n"
      "};\n"
      "\n";

    if (config_.use_compact_tbl())
      stream << "#define AMARU_USE_COMPACT_TBL\n\n";

    stream <<
      "static struct {\n"
      "  suint_t  const upper;\n"
      "  suint_t  const lower;\n"
      "} const multipliers[] = {\n";

    auto const nibbles = ssize / 4;

    auto e2      = info_.bin_exponent_min();
    auto e2_or_f = config_.use_compact_tbl() ? log10_pow2(e2) : e2;

    for (auto const& fast_eaf : fast_eafs) {

      integer_t upper, lower;
      divide_qr(fast_eaf.U, p2ssize, upper, lower);

      if (upper >= p2ssize)
        throw amaru_exception{"Multiplier is out of range."};

      stream << "  { " <<
        "0x" << std::hex << std::setw(nibbles) << std::setfill('0') << upper << ", "
        "0x" << std::hex << std::setw(nibbles) << std::setfill('0') << lower <<
        std::dec << " }, // " << e2_or_f << "\n";
      ++e2_or_f;
    }

    stream << "};\n"
      "\n"
      "static struct {\n"
      "  suint_t const multiplier;\n"
      "  suint_t const bound;\n"
      "} const minverse[] = {\n";

    auto const minverse5  = integer_t{p2ssize - (p2ssize - 1) / 5};
    auto multiplier = integer_t{1};
    auto p5 = integer_t{1};

    // Amaru checks whether is_multiple_of_pow5(C, f) for
    // 1. C  = 2 * mantissa + 1 <= 2 * mantissa_max + 1;
    // 2. C <= 2 * mantissa * 2^e * 5^{-f} <= 20 * mantissa_max;
    // 3. C  = 20 * mantissa_min * 2^e * 5^{-f} <= 200 * mantissa_min;
    // Hence, 200 * mantissa_max is a conservative bound, i.e.,
    // If 5^f > 200 * mantissa_max, then is_multiple_of_pow5(C, f) == false;
    for (int32_t f = 0; p5 <= 200 * info_.normal_mantissa_max(); ++f) {
      auto const bound = p2ssize / p5 - (f == 0);
      stream << "  { "
        "0x" << std::hex << std::setw(nibbles) << std::setfill('0') <<
        multiplier << ", " <<
        "0x" << std::hex << std::setw(nibbles) << std::setfill('0') <<
        bound <<
        " },\n";
      multiplier = (multiplier * minverse5) % p2ssize;
      p5 *= 5;
    }

    stream << "};\n"
      "\n"
      "#ifdef __cplusplus\n"
      "}\n"
      "#endif\n"
      "\n"
      "#define AMARU_FUNCTION " << info_.function() << "\n"
      "#include \"../include/amaru.h\"\n"
      "\n"
      "#undef AMARU_FUNCTION\n";

    if (config_.use_compact_tbl())
      stream << "#undef AMARU_USE_COMPACT_TBL\n";
  }

  /**
   * \brief Gets the maxima of all primary problems. (See get_maximum_primary.)
   *
   * It returns a vector v of size
   *     info_.bin_exponent_max() - info_.bin_exponent_min() + 1
   * such that v[i] contains the maximum of the primary problem corresponding to
   * exponent = info_.bin_exponent_min() + i.
   *
   * \returns The vector v.
   */
  std::vector<alpha_delta_maximum> get_maxima() const {

    std::vector<alpha_delta_maximum> maxima;
    maxima.reserve(info_.bin_exponent_max() - info_.bin_exponent_min() + 1);

    auto f_done = log10_pow2(info_.bin_exponent_min()) - 1;

    for (auto e2 = info_.bin_exponent_min(); e2 <= info_.bin_exponent_max();
      ++e2) {

      auto const f = log10_pow2(e2);

      if (config_.use_compact_tbl() && f == f_done)
        continue;

      auto const e = (config_.use_compact_tbl() ?
        e2 - log10_pow2_remainder(e2) : e2) - f;

      alpha_delta_maximum x;
      x.alpha   = f >= 0 ? pow2(e) : pow5(-f);
      x.delta   = f >= 0 ? pow5(f) : pow2(-e);
      x.maximum = get_maximum(x.alpha, x.delta, e2 == info_.bin_exponent_min());

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

    auto const mantissa_min = info_.normal_mantissa_min();
    auto const mantissa_max = info_.normal_mantissa_max();

    alpha %= delta;

    // Usual interval.

    auto const a = start_at_1 ? integer_t{1} : integer_t{2 * mantissa_min};
    auto const b = config_.use_compact_tbl() ?
        integer_t{16 * mantissa_max - 15} : integer_t{2 * mantissa_max};

    auto const max_ab = get_maximum_primary(alpha, delta, a, b);

    // Extras that are needed when mantissa == normal_mantissa_min().

    auto max_extras = [&](auto const& mantissa) {
      auto const m_a     =  4 * mantissa - 1;
      auto const m_c     = 20 * mantissa;
      auto const max_m_a = phi(alpha, delta, m_a);
      auto const max_m_c = phi(alpha, delta, m_c);
      return std::max(max_m_a, max_m_c);
    };

    if (!config_.use_compact_tbl())
      return std::max(max_ab, max_extras(mantissa_min));

    return std::max({max_ab, max_extras(mantissa_min),
      max_extras(2 * mantissa_min), max_extras(4 * mantissa_min),
      max_extras(8 * mantissa_min)});
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

    // Making shift >= ssize, simplifies multiply_and_shift executed at runtime.
    // Indeed, it ensures that the least significant limb of the product is
    // irrelevant. For this reason, later on, the generator actually outputs
    // shift - ssize (still labelling it as 'shift') so that Amaru doesn't need
    // to do it at runtime.
    auto k    = info_.ssize();
    auto pow2 = integer_t{1} << k;

    integer_t q, r;
    divide_qr(pow2 * x.alpha, x.delta, q, r);

    // It should return from inside the loop but let's set an upper bound.
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

  info_t      info_;
  config_t    config_;
  integer_t   p2ssize_;
  std::string dot_h_;
  std::string dot_c_;
};

int main() {

  try {

    auto const config = config_t{
      /* use_compact_tbl */ true,
      /* directory       */ "../generated"
    };

    auto const ieee32_info = info_t{
      /* id               */ "ieee32",
      /* suint            */ "uint32_t",
      /* duint            */ "uint64_t",
      /* ssize            */ 32,
      /* exponent_size    */ 8,
      /* bin_exponent_min */ -149,
      /* bin_exponent_max */ 104,
      /* mantissa_size    */ 23
    };
    auto generator_32 = generator_t{ieee32_info, config};
    generator_32.generate();

    auto const ieee64_info = info_t{
      /* id               */ "ieee64",
      /* suint            */ "uint64_t",
      /* duint            */ "__uint128_t",
      /* ssize            */ 64,
      /* exponent_size    */ 11,
      /* bin_exponent_min */ -1074,
      /* bin_exponent_max */ 971,
      /* mantissa_size    */ 52
    };
    auto generator_64 = generator_t{ieee64_info, config};
    generator_64.generate();
  }

  catch (amaru_exception const& e) {
    std::printf("Generation failed: %s\n", e.what());
  }

  catch (std::exception const& e) {
    std::printf("std::exception thrown: %s\n", e.what());
  }

  catch (...) {
    std::printf("Unknown exception thrown.\n");
  }
}
