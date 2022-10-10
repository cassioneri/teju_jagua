#include "amaru/common.h"

#include <boost/multiprecision/cpp_int.hpp>
#include <nlohmann/json.hpp>

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

namespace amaru {

/**
 * \brief Returns 2^n.
 */
integer_t
pow2(std::uint32_t n) {
  return integer_t{1} << n;
}

/**
 * \brief Returns 5^n.
 */
integer_t
pow5(std::uint32_t n) {
  if (n == 0)
    return 1;
  auto const p1 = pow5(n / 2);
  return p1 * p1 * (n % 2 == 0 ? 1 : 5);
}

/**
 * \brief Exception thrown by the generator.
 */
struct amaru_exception_t : std::range_error {
  using std::range_error::range_error;
};

/**
 * \brief Information about a floating point number type.
 */
struct info_t {

  // An identifier for the floating point number type (e.g., "ieee32" or
  // "ieee64".) This is used in C/C++ identifiers and, accordingly, the set of
  // accepted characters is defined by the C and C++ standards. In particular,
  // it must not contain spaces -- "long double" is forbidden.
  std::string id;

  // C/C++ name of the type used for mantissa storage and calculations (e.g.,
  // "uint32_t" or "uint64_t".) It might contain spaces -- "unsigned long" is
  // allowed.
  std::string suint;

  // C/C++ name of the type used for larger mantissa calculations (e.g.,
  // "uint64_t" or "uint128_t".) It must be, at least, double the size of suint.
  std::string duint;

  // The size of suint in bits (e.g., 64.)
  std::uint32_t ssize;

  // Size of exponent in bits (e.g., 11.)
  std::uint32_t exponent_size;

  // Minimum binary exponent (e.g., -1074.)
  std::int32_t bin_exponent_min;

  // Maximum binary exponent (e.g., 971.)
  std::int32_t bin_exponent_max;

  // Size of mantissa in bits (e.g., 52).
  std::uint32_t mantissa_size;

}; // struct info_t

void
to_json(nlohmann::json& json, info_t const& info) {
  json = nlohmann::json{
      {"id"              , info.id              },
      {"suint"           , info.suint           },
      {"duint"           , info.duint           },
      {"ssize"           , info.ssize           },
      {"exponent_size"   , info.exponent_size   },
      {"bin_exponent_min", info.bin_exponent_min},
      {"bin_exponent_max", info.bin_exponent_max},
      {"mantissa_size"   , info.mantissa_size   }
  };
}

void
from_json(nlohmann::json const& json, info_t& info) {
    json.at("id"              ).get_to(info.id              );
    json.at("suint"           ).get_to(info.suint           );
    json.at("duint"           ).get_to(info.duint           );
    json.at("ssize"           ).get_to(info.ssize           );
    json.at("exponent_size"   ).get_to(info.exponent_size   );
    json.at("bin_exponent_min").get_to(info.bin_exponent_min);
    json.at("bin_exponent_max").get_to(info.bin_exponent_max);
    json.at("mantissa_size"   ).get_to(info.mantissa_size   );
}

/**
 * \brief Configuration of Amaru's implementation.
 */
struct config_t {

  // Specifies if Amaru should use a compact table of multipliers.
  bool is_compact;

  // Directory where generated files are saved.
  std::string directory;

}; // struct config_t

void
to_json(nlohmann::json& json, config_t const& config) {
  json = nlohmann::json{
    {"is_compact", config.is_compact}
  };
}

void
from_json(nlohmann::json const& json, config_t& config) {
  json.at("is_compact").get_to(config.is_compact);
}

/**
 * \brief Generator of Amaru's implementation for a given floating point number
 * type.
 */
struct generator_t {

  /**
   * \brief Constructor.
   *
   * \param info            The information on the floating point number type.
   * \param config          The implementation configuration.
   */
  generator_t(info_t info, config_t config) :
    info_               {std::move(info)                           },
    config_             {std::move(config)                         },
    function_           {"amaru_bin_to_dec_" + info_.id            },
    rep_                {info_.id + "_t"                           },
    dec_exponent_min_   {log10_pow2(info_.bin_exponent_min)        },
    normal_mantissa_min_{AMARU_POW2(integer_t, info_.mantissa_size)},
    normal_mantissa_max_{2 * normal_mantissa_min_                  },
    p2ssize_            {integer_t{1} << info_.ssize               },
    dot_h_              {info_.id + ".h"                           },
    dot_c_              {info_.id + ".c"                           } {
  }

  /**
   * \brief Returns the identifier for the floating point number type.
   */
  std::string const&
  id() const {
    return info_.id;
  }

  /**
   * \brief Returns the C/C++ name of the type used for mantissa storage and
   * calculations.
   */
  std::string const&
  suint() const {
    return info_.suint;
  }

  /**
   * \brief Returns the C/C++ name of the type used for larger mantissa
   * calculations.
   */
  std::string const&
  duint() const {
    return info_.duint;
  }

  /**
   * \brief Returns the size of suint in bits.
   */
  std::uint32_t const&
  ssize() const {
    return info_.ssize;
  }

  /**
   * \brief Returns the size of exponent in bits.
   */
  std::uint32_t const&
  exponent_size() const {
    return info_.exponent_size;
  }

  /**
   * \brief Returns the minimum binary exponent.
   */
  std::int32_t const&
  bin_exponent_min() const {
    return info_.bin_exponent_min;
  }

  /**
   * \brief Returns maximum binary exponent.
   */
  std::int32_t const&
  bin_exponent_max() const {
    return info_.bin_exponent_max;
  }

  /**
   * \brief Returns the size of mantissa in bits.
   */
  std::uint32_t const&
  mantissa_size() const {
    return info_.mantissa_size;
  }

  /**
   * \brief Returns the name of Amaru's conversion function.
   */
  std::string const&
  function() const {
    return function_;
  }

  /**
   * \brief Returns the C/C++ name of Amaru's representation type.
   */
  std::string const&
  rep() const {
    return rep_;
  }

  /**
   * \brief Returns the decimal minimum exponent.
   */
  std::int32_t const&
  dec_exponent_min() const {
    return dec_exponent_min_;
  }

  /**
   * \brief Returns the normal (inclusive) minimal mantissa.
   *
   * Mantissas for normal floating point numbers are elements of the interval
   * [normal_mantissa_min(), normal_mantissa_max()[.
   */
  integer_t const&
  normal_mantissa_min() const {
    return normal_mantissa_min_;
  }

  /**
   * \brief Returns the normal (exclusive) maximal mantissa.
   *
   * Mantissas for normal floating point numbers are elements of the interval
   * [normal_mantissa_min(), normal_mantissa_max()[.
   */
  integer_t const&
  normal_mantissa_max() const {
    return normal_mantissa_max_;
  }

  /**
   * \brief Returns whether using a compact table of multipliers.
   */
  bool
  is_compact() const {
    return config_.is_compact;
  }

  /**
   * \brief Returns the directory where generated files are saved.
   */
  std::string const&
  directory() const {
    return config_.directory;
  }

  /**
   * \brief Returns the name of the generated .h file.
   */
  std::string const&
  dot_h() const {
    return dot_h_;
  }

  /**
   * \brief Returns the name of the generated .c file.
   */
  std::string const&
  dot_c() const {
    return dot_c_;
  }

  /**
   * \brief Generates the declaration and implementation.
   */
  void
  generate() const {

    auto dot_h_stream = std::ofstream{directory() + dot_h()};
    auto dot_c_stream = std::ofstream{directory() + dot_c()};

    std::cout << "Generation started.\n";

    // Overflow check 1:
    if (2 * normal_mantissa_max() + 1 >= p2ssize_)
      throw amaru_exception_t("suint_t is not large enough for calculations to "
        "not overflow.");

    // Overflow check 2:
    if (20 * normal_mantissa_min() >= p2ssize_)
      throw amaru_exception_t("suint_t is not large enough for calculations to "
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
    integer_t     U;
    std::uint32_t k;
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
  static rational_t
  phi(integer_t const& alpha, integer_t const& delta,
    integer_t const& m) {
    return {m, delta - alpha * m % delta};
  }

  /**
   * \brief The objective function of the secondary maximisation problem:
   *
   *     phi'(m') := m' / (1 + (alpha' * m' - 1) % delta').
   */
  static rational_t
  phi_p(integer_t const& alpha, integer_t const& delta,
    integer_t const& m) {
    return {m, 1 + (alpha * m - 1) % delta};
  }

  /**
   * \brief Streams out the .h file.
   *
   * \param stream Output stream to receive the content.
   */
  void
  generate_dot_h(std::ostream& stream) const {

    stream <<
      "// This file was auto-generated. DO NOT EDIT IT.\n"
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
      "  bool is_negative;\n"
      "  int32_t exponent;\n"
      "  " << suint() << " mantissa;\n"
      "} " << rep() << ";\n"
      "\n" <<
        rep() << ' ' << function() << "(bool is_negative, "
        "int32_t exponent, " << suint() << " mantissa);\n"
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
  void
  generate_dot_c(std::ostream& stream) const {

    stream << "// This file was auto-generated. DO NOT EDIT IT.\n"
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
    if (is_compact())
      shift = 2 * ssize() - 1;

    // Replace minimal fast EAFs to use the same shift.

    auto const p2shift = integer_t{1} << shift;

    for (uint32_t i = 0; i < maxima.size(); ++i) {

      auto const& x = maxima[i];

      integer_t q, r;
      divide_qr(x.alpha << shift, x.delta, q, r);

      if (x.maximum >= rational_t{p2shift, x.delta - r})
        throw amaru_exception_t{"Unable to use same shift."};

      fast_eafs[i] = fast_eaf_t{q + 1, shift};
    }

    auto const p2ssize = integer_t{1} << ssize();

    stream <<
      "typedef " << suint() << " suint_t;\n"
      "typedef " << duint() << " duint_t;\n"
      "typedef " << rep()   << " rep_t;\n"
      "\n"
      "enum {\n"
      "  is_compact       = " << is_compact()       << ",\n"
      "  ssize            = " << ssize()            << ",\n"
      "  mantissa_size    = " << mantissa_size()    << ",\n"
      "  bin_exponent_min = " << bin_exponent_min() << ",\n"
      "  dec_exponent_min = " << dec_exponent_min() << ",\n"
      // Instead of Amaru dividing multipliy_and_shift(m_a, upper, lower) by 2
      // we increment the shift here so this has the same effect.
      "  shift            = " << shift + 1                << "\n"
      "};\n"
      "\n";

    stream <<
      "static struct {\n"
      "  suint_t  const upper;\n"
      "  suint_t  const lower;\n"
      "} const multipliers[] = {\n";

    auto const nibbles = ssize() / 4;

    auto e2      = bin_exponent_min();
    auto e2_or_f = is_compact() ? log10_pow2(e2) : e2;

    for (auto const& fast_eaf : fast_eafs) {

      integer_t upper, lower;
      divide_qr(fast_eaf.U, p2ssize, upper, lower);

      if (upper >= p2ssize)
        throw amaru_exception_t{"Multiplier is out of range."};

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
    for (int32_t f = 0; p5 <= 200 * normal_mantissa_max(); ++f) {
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
      "#define AMARU_FUNCTION " << function() << "\n"
      "#include \"amaru/amaru.h\"\n"
      "\n"
      "#undef AMARU_FUNCTION\n";
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
  std::vector<alpha_delta_maximum>
  get_maxima() const {

    std::vector<alpha_delta_maximum> maxima;
    maxima.reserve(bin_exponent_max() - bin_exponent_min() + 1);

    auto f_done = log10_pow2(bin_exponent_min()) - 1;

    for (auto e2 = bin_exponent_min(); e2 <= bin_exponent_max(); ++e2) {

      auto const f = log10_pow2(e2);

      if (is_compact() && f == f_done)
        continue;

      auto const e = (is_compact() ? e2 - log10_pow2_remainder(e2) : e2) - f;

      alpha_delta_maximum x;
      x.alpha   = f >= 0 ? pow2(e) : pow5(-f);
      x.delta   = f >= 0 ? pow5(f) : pow2(-e);
      x.maximum = get_maximum(x.alpha, x.delta, e2 == bin_exponent_min());

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
  static rational_t
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

  /**
   * \brief Given alpha', delta', a' and b', this function calculates the
   * maximiser of phi'(m') over [a', b'[.
   *
   * \pre 0 < alpha' && 0 < delta' && 1 <= a' && a' < b'.
   */
  static rational_t
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

  /**
   * \brief Given alpha and delta, this function calculates the maximiser of
   * phi(m) over the relevant set of mantissas.
   *
   * \pre 0 <= alpha && 0 < delta.
   */
  rational_t
  get_maximum(integer_t alpha, integer_t const& delta, bool start_at_1 = false)
    const {

    auto const mantissa_min = normal_mantissa_min();
    auto const mantissa_max = normal_mantissa_max();

    alpha %= delta;

    // Usual interval.

    auto const a = start_at_1 ? integer_t{1} : integer_t{2 * mantissa_min};
    auto const b = is_compact() ? integer_t{16 * mantissa_max - 15} :
      integer_t{2 * mantissa_max};

    auto const max_ab = get_maximum_primary(alpha, delta, a, b);

    // Extras that are needed when mantissa == normal_mantissa_min().

    auto max_extras = [&](auto const& mantissa) {
      auto const m_a     =  4 * mantissa - 1;
      auto const m_c     = 20 * mantissa;
      auto const max_m_a = phi(alpha, delta, m_a);
      auto const max_m_c = phi(alpha, delta, m_c);
      return std::max(max_m_a, max_m_c);
    };

    if (!is_compact())
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
  fast_eaf_t
  get_fast_eaf(alpha_delta_maximum const& x) const {

    // Making shift >= ssize, simplifies multiply_and_shift executed at runtime.
    // Indeed, it ensures that the least significant limb of the product is
    // irrelevant. For this reason, later on, the generator actually outputs
    // shift - ssize (still labelling it as 'shift') so that Amaru doesn't need
    // to do it at runtime.
    auto k    = ssize();
    auto pow2 = integer_t{1} << k;

    integer_t q, r;
    divide_qr(pow2 * x.alpha, x.delta, q, r);

    // It should return from inside the loop but let's set an upper bound.
    while (k < 3 * ssize()) {

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

    throw amaru_exception_t{"Cannot find fast EAF."};
  }

  info_t       info_;
  config_t     config_;
  std::string  function_;
  std::string  rep_;
  std::int32_t dec_exponent_min_;
  integer_t    normal_mantissa_min_;
  integer_t    normal_mantissa_max_;
  integer_t    p2ssize_;
  std::string  dot_h_;
  std::string  dot_c_;
};

void
report_usage(FILE* const stream, const char* const prog) noexcept {
  std::fprintf(stream, "Usage: %s [OPTION]... CONFIG DIR\n"
    "Generate Amaru source files for the given JSON configuration file CONFIG. "
    "The files are saved in directory DIR.\n"
    "\n"
    "Options:\n"
    "  --h        shows this message and exits.\n",
    prog);
}

void
report_error(const char* const prog, const char* const msg) noexcept {
  std::fprintf(stderr, "%s: error: %s.\n", prog, msg);
  report_usage(stderr, prog);
  std::exit(-1);
}

generator_t
parse(const char* const filename, const char* const dir) {

  std::ifstream file(filename);
  auto const data = nlohmann::json::parse(file);

  auto info   = data["info"  ].get<info_t  >();
  auto config = data["config"].get<config_t>();
  config.directory = dir;
  if (config.directory.back() != '/')
    config.directory.append(1, '/');

  return { std::move(info), std::move(config) };
}

} // namespace amaru

int
main(int const argc, const char* const argv[]) {

  using namespace amaru;

  if (argc != 3)
    report_error(argv[0], "expected two arguments");

  try {

    auto const generator = parse(argv[1], argv[2]);
    generator.generate();

  }

  catch (amaru_exception_t const& e) {
    report_error(argv[0], e.what());
  }

  catch (std::exception const& e) {
    report_error(argv[0], e.what());
  }

  catch (...) {
    report_error(argv[0], "unknown error");
  }
}
