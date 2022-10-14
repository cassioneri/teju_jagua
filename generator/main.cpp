#include "amaru/common.h"
#include "generator/config.hpp"
#include "generator/exception.hpp"

#include <boost/multiprecision/cpp_int.hpp>
#include <nlohmann/json.hpp>

#include <algorithm>
#include <cctype>
#include <climits>
#include <cstdint>
#include <cstdio>
#include <iomanip>
#include <ios>
#include <iterator>
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
  generator_t(config_t config, std::string directory) :
    config_             {std::move(config)                     },
    directory_          {std::move(directory)                  },
    compact_or_full_    {is_compact() ? "compact" : "full"     },
    function_           {"amaru_binary_to_decimal_"
      + id() + "_" + compact_or_full()                         },
    fields_             {get_fields(size())                    },
    dec_exponent_min_   {log10_pow2(bin_exponent_min())        },
    normal_mantissa_min_{AMARU_POW2(integer_t, mantissa_size())},
    normal_mantissa_max_{2 * normal_mantissa_min()             },
    p2_size_            {integer_t{1} << size()                },
    dot_h_              {id() + "_" + compact_or_full() + ".h" },
    dot_c_              {id() + "_" + compact_or_full() + ".c" } {
  }

  /**
   * \brief Returns the identifier for the floating point number type.
   */
  std::string const&
  id() const {
    return config_.id;
  }

  /**
   * \brief Returns the size of the limb in bits.
   */
  std::uint32_t
  size() const {
    return config_.size;
  }

  /**
   * \brief Returns the size of exponent in bits.
   */
  std::uint32_t const&
  exponent_size() const {
    return config_.exponent.size;
  }

  /**
   * \brief Returns the minimum binary exponent.
   */
  std::int32_t const&
  bin_exponent_min() const {
    return config_.exponent.minimum;
  }

  /**
   * \brief Returns maximum binary exponent.
   */
  std::int32_t const&
  bin_exponent_max() const {
    return config_.exponent.maximum;
  }

  /**
   * \brief Returns the size of mantissa in bits.
   */
  std::uint32_t const&
  mantissa_size() const {
    return config_.mantissa.size;
  }

  std::string const&
  compact_or_full() const {
    return compact_or_full_;
  }

  /**
   * \brief Returns the name of Amaru's conversion function.
   */
  std::string const&
  function() const {
    return function_;
  }

  /**
   * \brief Returns the C/C++ name of Amaru's fields type.
   */
  std::string const&
  fields() const {
    return fields_;
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
    return config_.storage.exponent == base_t::decimal;
  }

  /**
   * \brief Returns the directory where generated files are saved.
   */
  std::string const&
  directory() const {
    return directory_;
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
    if (2 * normal_mantissa_max() + 1 >= p2_size_)
      throw exception_t("The limb is not large enough for calculations to "
        "not overflow.");

    // Overflow check 2:
    if (20 * normal_mantissa_min() >= p2_size_)
      throw exception_t("The limb is not large enough for calculations to "
        "not overflow.");

    std::cout << "  Generating \"" << dot_h() << "\".\n";
    generate_dot_h(dot_h_stream);

    std::cout << "  Generating \"" << dot_c() << "\".\n";
    generate_dot_c(dot_c_stream);

    std::cout << "Generation finished.\n";
  }

private:

  std::string get_fields(uint32_t size) {
    switch (size) {
      case 32:
        return "amaru32_fields_t";
      case 64:
        return "amaru64_fields_t";
    }
    throw exception_t{"Size must be in {32, 64}."};
  };

  /**
   * \brief Converts a given string to upper case letters.
   */
  static std::string to_upper(std::string const& str) {
    std::string result;
    std::transform(str.begin(), str.end(), std::back_inserter(result),
      [](char const c) { return static_cast<char>(std::toupper(c)); });
    return result;
  }

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

    std::string const include_guard = "AMARU_AMARU_GENERATED_" + to_upper(id())
      + "_" + to_upper(compact_or_full()) + "_H_";

    stream <<
      "// This file was auto-generated. DO NOT EDIT IT.\n"
      "\n"
      "#ifndef " << include_guard << "\n"
      "#define " << include_guard << "\n"
      "\n"
      "#include \"amaru/types.h\"\n"
      "\n"
      "#include <stdbool.h>\n"
      "#include <stdint.h>\n"
      "\n"
      "#ifdef __cplusplus\n"
      "extern \"C\" {\n"
      "#endif\n"
      "\n" <<
        fields() << ' ' << function() << "(bool is_negative, "
        "int32_t exponent, amaru" << size() << "_limb1_t mantissa);\n"
      "\n" <<
      "#ifdef __cplusplus\n"
      "}\n"
      "#endif\n"
      "\n"
      "#endif // " << include_guard <<
      "\n";
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
      "\n"
      "#define AMARU_MAX_LIMBS AMARU" << size() << "_MAX_LIMBS\n"
      "\n"
      "typedef amaru" << size() << "_limb1_t amaru_limb1_t;\n"
      "#if AMARU" << size() << "_MAX_LIMBS >= 2\n"
      "typedef amaru" << size() << "_limb2_t amaru_limb2_t;\n"
      "#elif AMARU" << size() << "_MAX_LIMBS >= 4\n"
      "typedef amaru" << size() << "_limb4_t amaru_limb4_t;\n"
      "#endif\n"
      "\n"
      "typedef amaru" << size() << "_fields_t amaru_fields_t;\n"
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
      shift = 2 * size() - 1;

    // Replace minimal fast EAFs to use the same shift.

    auto const p2shift = integer_t{1} << shift;

    for (std::uint32_t i = 0; i < maxima.size(); ++i) {

      auto const& x = maxima[i];

      integer_t q, r;
      divide_qr(x.alpha << shift, x.delta, q, r);

      if (x.maximum >= rational_t{p2shift, x.delta - r})
        throw exception_t{"Unable to use same shift."};

      fast_eafs[i] = fast_eaf_t{q + 1, shift};
    }

    auto const p2_size = integer_t{1} << size();

    stream <<
      "enum {\n"
      "  is_compact       = " << is_compact()       << ",\n"
      "  size             = " << size()             << ",\n"
      "  mantissa_size    = " << mantissa_size()    << ",\n"
      "  bin_exponent_min = " << bin_exponent_min() << ",\n"
      "  dec_exponent_min = " << dec_exponent_min() << ",\n"
      // Instead of Amaru dividing multipliy_and_shift(m_a, upper, lower) by 2
      // we increment the shift here so this has the same effect.
      "  shift            = " << shift + 1          << "\n"
      "};\n"
      "\n";

    stream <<
      "static struct {\n"
      "  amaru_limb1_t const upper;\n"
      "  amaru_limb1_t const lower;\n"
      "} const multipliers[] = {\n";

    auto const nibbles = size() / 4;

    auto e2      = bin_exponent_min();
    auto e2_or_f = is_compact() ? log10_pow2(e2) : e2;

    for (auto const& fast_eaf : fast_eafs) {

      integer_t upper, lower;
      divide_qr(fast_eaf.U, p2_size, upper, lower);

      if (upper >= p2_size)
        throw exception_t{"Multiplier is out of range."};

      stream << "  { " <<
        "0x" << std::hex << std::setw(nibbles) << std::setfill('0') << upper <<
        ", "
        "0x" << std::hex << std::setw(nibbles) << std::setfill('0') << lower <<
        std::dec << " }, // " << e2_or_f << "\n";
      ++e2_or_f;
    }

    stream << "};\n"
      "\n"
      "static struct {\n"
      "  amaru_limb1_t const multiplier;\n"
      "  amaru_limb1_t const bound;\n"
      "} const minverse[] = {\n";

    auto const minverse5  = integer_t{p2_size - (p2_size - 1) / 5};
    auto multiplier = integer_t{1};
    auto p5 = integer_t{1};

    // Amaru checks whether is_multiple_of_pow5(C, f) for
    // 1. C  = 2 * mantissa + 1 <= 2 * mantissa_max + 1;
    // 2. C <= 2 * mantissa * 2^e * 5^{-f} <= 20 * mantissa_max;
    // 3. C  = 20 * mantissa_min * 2^e * 5^{-f} <= 200 * mantissa_min;
    // Hence, 200 * mantissa_max is a conservative bound, i.e.,
    // If 5^f > 200 * mantissa_max, then is_multiple_of_pow5(C, f) == false;
    for (int32_t f = 0; p5 <= 200 * normal_mantissa_max(); ++f) {
      auto const bound = p2_size / p5 - (f == 0);
      stream << "  { "
        "0x" << std::hex << std::setw(nibbles) << std::setfill('0') <<
        multiplier << ", " <<
        "0x" << std::hex << std::setw(nibbles) << std::setfill('0') <<
        bound <<
        " },\n";
      multiplier = (multiplier * minverse5) % p2_size;
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
      "#undef AMARU_FUNCTION\n"
      "#undef AMARU_MAX_LIMBS\n";
  }

  /**
   * \brief Gets the maxima of all primary problems. (See get_maximum_primary.)
   *
   * It returns a vector v of size
   *     bin_exponent_max() - bin_exponent_min() + 1
   * such that v[i] contains the maximum of the primary problem corresponding to
   * exponent = bin_exponent_min() + i.
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

    // Making shift >= size, simplifies multiply_and_shift executed at runtime.
    // Indeed, it ensures that the least significant limb of the product is
    // irrelevant. For this reason, later on, the generator actually outputs
    // shift - size (still labelling it as 'shift') so that Amaru doesn't need
    // to do it at runtime.
    auto k    = size();
    auto pow2 = integer_t{1} << k;

    integer_t q, r;
    divide_qr(pow2 * x.alpha, x.delta, q, r);

    // It should return from inside the loop but let's set an upper bound.
    while (k < 3 * size()) {

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

    throw exception_t{"Cannot find fast EAF."};
  }

  config_t     config_;
  std::string  directory_;
  std::string  compact_or_full_;
  std::string  function_;
  std::string  fields_;
  std::int32_t dec_exponent_min_;
  integer_t    normal_mantissa_min_;
  integer_t    normal_mantissa_max_;
  integer_t    p2_size_;
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

  auto config = data.get<config_t>();

  std::string directory = dir;
  if (directory.back() != '/')
    directory.append(1, '/');

  return { std::move(config), std::move(directory) };
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

  catch (exception_t const& e) {
    report_error(argv[0], e.what());
  }

  catch (std::exception const& e) {
    report_error(argv[0], e.what());
  }

  catch (...) {
    report_error(argv[0], "unknown error");
  }
}
