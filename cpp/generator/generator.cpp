#include "amaru/common.h"
#include "cpp/common/exception.hpp"
#include "cpp/generator/generator.hpp"

#include <boost/multiprecision/cpp_int.hpp>

#include <algorithm>
#include <fstream>

namespace amaru {

//------------------------------------------------------------------------------
// helpers
//------------------------------------------------------------------------------

namespace {

using integer_t  = boost::multiprecision::cpp_int;
using rational_t = boost::multiprecision::cpp_rational;

/**
 * \brief Returns 2^n.
 */
integer_t
pow2(std::uint32_t const n) {
  return integer_t{1} << n;
}

/**
 * \brief Returns 5^n.
 */
integer_t
pow5(std::uint32_t const n) {
  if (n == 0)
    return 1;
  auto const p1 = pow5(n / 2);
  return p1 * p1 * (n % 2 == 0 ? 1 : 5);
}

/**
 * \brief Splits a number into smaller pieces.
 *
 * Generated sources might need to hardcode large numbers but the platform might
 * lack support for literals of the required size. This class helps splitting
 * such number into smaller pieces which can be given to macros amaru_pack2 or
 * amaru_pack2 for the large number to be reconstructed.
 *
 * A typical usage looks like this:
 *
 *     splitter_t splitter{128, 2};
 *     integer_t n = (integer_t{0x0123456789abcdef} << 64) + 0xfedcba9876543210;
 *     std::cout << splitter(n) << '\n';
 *
 * Which streams:
 *
 *     amaru_pack2(0x0123456789abcdef, 0xfedcba9876543210)
 */
struct splitter_t {

  /**
   * \brief Constructor.
   *
   * \param size              The limb size to be splitted.
   * \param parts             Number of parts that the limbs must be split into.
   */
  splitter_t(std::uint32_t size, std::uint32_t parts) :
    size {std::move(size) },
    parts{std::move(parts)} {
  }

  struct data_t;

  /**
   * \brief Returns an object which, when streamed out,  splits n.
   */
  data_t
  operator()(integer_t n) const;

  std::uint32_t size;
  std::uint32_t parts;
};

/**
 * \brief Created by splitter_t::operator(), it holds information for splitting
 * a given number.
 */
struct splitter_t::data_t {

  /**
   * \brief Constructor.
   *
   * \param splitter          The splitter that created *this.
   * \param n                 The number to be split.
   */
  data_t(splitter_t splitter, integer_t n) :
    splitter{splitter    },
    n       {std::move(n)} {
  }

  splitter_t splitter;
  integer_t  n;
};

splitter_t::data_t
splitter_t::operator()(integer_t n) const {
  return {*this, std::move(n)};
}

/**
 * \brief The operator for data.
 *
 * At the time of construction, data received a splitter and a number n. This
 * stream operator uses splitter's fields to configure the splitting of n.
 *
 * \param os                  The object to be streamed to.
 * \param data                The data taken by r-value reference.
 */
std::ostream&
operator<<(std::ostream& os, splitter_t::data_t&& data) {

  if (data.splitter.parts == 1)
    return os << "0x" << std::hex << std::setw(data.splitter.size / 4) <<
      std::setfill('0') << data.n;

  auto const sub_size = data.splitter.size / data.splitter.parts;
  auto       k        = data.splitter.parts - 1;
  auto       base     = integer_t{1} << (k * sub_size);
  integer_t u;

  os << "amaru_pack" << data.splitter.parts << '(';

  goto skip_comma;
  while (k) {

    --k;
    base >>= sub_size;

    os << ", ";
    skip_comma:

    divide_qr(data.n, base, u, data.n);

    os << "0x" << std::hex << std::setw(sub_size / 4) <<
      std::setfill('0') << u;
  }

  return os << ')';
}

/**
 * \brief The objective function of the primary maximisation problem:
 *
 *     phi_1(m) := m / (delta_1 - alpha_1 * m % delta_1).
 *
 * \param alpha_1           Parameter alpha_1.
 * \param delta_1           Parameter delta_1.
 * \param m                 Variable m.
 */
rational_t
phi_1(integer_t const& alpha_1, integer_t const& delta_1,
  integer_t const& m) {
  return {m, delta_1 - alpha_1 * m % delta_1};
}

/**
 * \brief The objective function of the secondary maximisation problem:
 *
 *     phi_2(m) := m / (1 + (alpha_2 * m - 1) % delta_2).
 *
 * \param alpha_2           Parameter alpha_2.
 * \param delta_2           Parameter delta_2.
 * \param m                 Variable m.
 */
rational_t
phi_2(integer_t const& alpha_2, integer_t const& delta_2,
  integer_t const& m) {
  return {m, 1 + (alpha_2 * m - 1) % delta_2};
}

/**
 * \brief Given alpha_1, delta_1, a_1 and b_1, this function calculates the
 * maximiser of phi_1(m) over [a_1, b_1[.
 *
 * \pre 0 <= alpha_1 && alpha_1 < delta_1 && a_1 < b_1.
 *
 * \param alpha_1           Parameter alpha_1.
 * \param delta_1           Parameter delta_1.
 * \param a_1               Lower bound a_1.
 * \param b_1               Upper bound b_1.
 */
rational_t
get_maximum_1(integer_t const& alpha_1, integer_t const& delta_1,
  integer_t const& a_1, integer_t const& b_1);

/**
 * \brief Given alpha, delta, a_2 and b_2, this function calculates the
 * maximiser of phi_2(m) over [a_2, b_2[.
 *
 * \pre 0 < alpha_2 && 0 < delta_2 && 1 <= a_2 && a_2 < b_2.
 *
 * \param alpha_2           Parameter alpha_1.
 * \param delta_2           Parameter delta_1.
 * \param a_2               Lower bound a_2.
 * \param b_2               Upper bound b_2.
 */
rational_t
get_maximum_2(integer_t const& alpha_2, integer_t const& delta_2,
  integer_t const& a_2, integer_t const& b_2);

rational_t
get_maximum_1(integer_t const& alpha_1, integer_t const& delta_1,
  integer_t const& a_1, integer_t const& b_1) {

  auto const b_minus_1 = b_1 - 1;
  auto const maximum1  = phi_1(alpha_1, delta_1, b_minus_1);

  if (alpha_1 == 0 || a_1 == b_minus_1)
    return maximum1;

  auto const a_2 = alpha_1 * a_1 / delta_1 + 1;
  auto const b_2 = alpha_1 * b_minus_1 / delta_1 + 1;

  if (a_2 == b_2)
    return maximum1;

  auto const  alpha_2 = delta_1 % alpha_1;
  auto const& delta_2 = alpha_1;
  auto const  other   = get_maximum_2(alpha_2, delta_2, a_2, b_2);

  auto const  maximum2 = rational_t{
    delta_1 * numerator(other) - denominator(other),
      alpha_1 * denominator(other)};

  return std::max(maximum1, maximum2);
}

rational_t
get_maximum_2(integer_t const& alpha_2,
  integer_t const& delta_2, integer_t const& a_2, integer_t const& b_2) {

  if (alpha_2 == 0)
    return b_2 - 1;

  auto const maximum1 = phi_2(alpha_2, delta_2, a_2);

  if (a_2 == b_2 - 1)
    return maximum1;

  auto const a1 = (alpha_2 * a_2 - 1) / delta_2 + 1;
  auto const b1 = (alpha_2 * (b_2 - 1) - 1) / delta_2 + 1;

  if (a1 == b1)
    return maximum1;

  auto const  alpha1 = delta_2 % alpha_2;
  auto const& delta1 = alpha_2;
  auto const  other  = get_maximum_1(alpha1, delta1, a1, b1);

  auto const  maximum2 = rational_t{
    delta_2 * numerator(other) + denominator(other),
    alpha_2 * denominator(other)};

  return std::max(maximum1, maximum2);
}

/**
 * \brief Returns the type prefix corresponding to a given size.
 *
 * \param size              The size.
 *
 * \returns The type prefix corresponding to a given size.
 */
std::string
get_prefix(std::uint32_t const size) {
  switch (size) {
    case 32:
      return "amaru32_";
    case 64:
      return "amaru64_";
    case 128:
      return "amaru128_";
  }
  throw exception_t{"BUG: Invalid size."};
}

/**
 * \brief Converts a given string to upper case letters.
 *
 * \param str                The given string
 *
 * \returns The upper case string.
 */
std::string
to_upper(std::string const& str) {
  std::string result;
  std::transform(str.begin(), str.end(), std::back_inserter(result),
    [](char const c) { return static_cast<char>(std::toupper(c)); });
  return result;
}

/**
 * \brief Fast EAF coefficients.
 *
 * For given alpha > 0 and delta > 0, we often find U > 0 and k >= 0 such that:
 *
 *   alpha * m / delta = U * m >> k for m in a certain interval.
 *
 * This type stores U and k.
 */
struct fast_eaf_t {
  integer_t     U;
  std::uint32_t k;
};

/**
 * \brief Stores alpha, delta (usually pow2(e) and pow2(f)) and the maximum of
 * m / (delta - alpha * m % delta) for m in the set of mantissas.
 */
struct alpha_delta_maximum_t {
  integer_t  alpha;
  integer_t  delta;
  rational_t maximum;
};

std::int32_t
get_index_offset(std::uint32_t const base, std::int32_t const exponent_min) {
  return base == 10 ? amaru_log10_pow2(exponent_min) : exponent_min;
}

} // namespace <anonymous>

//------------------------------------------------------------------------------
// generator_t::impl_t
//------------------------------------------------------------------------------

struct generator_t::impl_t {

  impl_t(generator_t const* self) :
    self{*self} {
  }

  /**
   * \brief Returns the identifier for the floating point number type.
   */
  std::string const&
  id() const {
    return self.config_.id;
  }

  /**
   * \brief Returns the size of the limb in bits.
   */
  std::uint32_t
  size() const {
    return self.config_.size;
  }

  /**
   * \brief Returns the type prefix corresponding to a given size.
   */
  std::string const&
  prefix() const {
    return self.prefix_;
  }

  /**
   * \brief Returns the name of Amaru's conversion function.
   */
  std::string const&
  function() const {
    return self.function_;
  }

  /**
   * \brief Returns the minimum binary exponent.
   */
  std::int32_t
  exponent_min() const {
    return self.config_.exponent.minimum;
  }

  /**
   * \brief Returns maximum binary exponent.
   */
  std::int32_t
  exponent_max() const {
    return self.config_.exponent.maximum;
  }

  /**
   * \brief Returns the size of mantissa in bits.
   */
  std::uint32_t
  mantissa_size() const {
    return self.config_.mantissa.size;
  }

  /**
   * \brief Returns the normal (inclusive) minimal mantissa.
   *
   * Mantissas for normal floating point numbers are elements of the interval
   * [normal_mantissa_min(), normal_mantissa_max()[.
   */
  integer_t const&
  mantissa_min() const {
    return self.mantissa_min_;
  }

  /**
   * \brief Returns the normal (exclusive) maximal mantissa.
   *
   * Mantissas for normal floating point numbers are elements of the interval
   * [normal_mantissa_min(), normal_mantissa_max()[.
   */
  integer_t const&
  mantissa_max() const {
    return self.mantissa_max_;
  }

  /**
   * \brief Returns the base of the stored exponent.
   */
  std::uint32_t
  storage_base() const {
    return self.config_.storage.base;
  }

  /**
   * \brief Returns the number of parts that each stored limb is split into.
   */
  std::uint32_t
  storage_split() const {
    return self.config_.storage.split;
  }

  /**
   * \brief Returns the index offset.
   */
  std::int32_t
  index_offset() const {
    return self.index_offset_;
  }

  /**
   * \brief Returns the calculation method for div10.
   */
  std::string const&
  calculation_div10() const {
    return self.config_.calculation.div10;
  }

  /**
   * \brief Returns the calculation method for mshift.
   */
  std::string const&
  calculation_mshift() const {
    return self.config_.calculation.mshift;
  }

  /**
   * \brief Optimises for integers.
   */
  bool
  optimise_integer() const {
    return self.config_.optimisation.integer;
  }

  /**
   * \brief Optimises for mid points.
   */
  bool
  optimise_midpoint() const {
    return self.config_.optimisation.mid_point;
  }

  /**
   * \brief Returns the directory where generated files are saved.
   */
  std::string const&
  directory() const {
    return self.directory_;
  }

  /**
   * \brief Returns the name of the generated .h file.
   */
  std::string const&
  dot_h() const {
    return self.dot_h_;
  }

  /**
   * \brief Returns the name of the generated .c file.
   */
  std::string const&
  dot_c() const {
    return self.dot_c_;
  }

  /**
   * \brief Streams out the .h file.
   *
   * \param stream Output stream to receive the content.
   */
  void
  generate_dot_h(std::ostream& stream) const {

    std::string const include_guard = "AMARU_AMARU_GENERATED_" +
      to_upper(id()) + "_H_";

    stream <<
      "// This file was auto-generated. DO NOT EDIT IT.\n"
      "\n"
      "#ifndef " << include_guard << "\n"
      "#define " << include_guard << "\n"
      "\n"
      "#include <stdbool.h>\n"
      "#include <stdint.h>\n"
      "\n"
      "#include \"amaru/config.h\"\n"
      "\n"
      "#ifdef __cplusplus\n"
      "extern \"C\" {\n"
      "#endif\n"
      "\n" << prefix() << "fields_t\n" <<
      function() << '(' << prefix() << "fields_t binary);\n"
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
   * \param stream          Output stream to receive the content.
   */
  void
  generate_dot_c(std::ostream& stream) const {

    stream << "// This file was auto-generated. DO NOT EDIT IT.\n"
      "\n"
      "#include \"" << dot_h() << "\"\n"
      "\n"
      "#include \"amaru/pack.h\"\n"
      "\n"
      "#ifdef __cplusplus\n"
      "extern \"C\" {\n"
      "#endif\n"
      "\n";

    auto const maxima = get_maxima();
    std::vector<fast_eaf_t> fast_eafs;
    fast_eafs.reserve(maxima.size());

    auto shift = std::uint32_t{0};

    // Calculates minimal fast EAFs (i.e., those with minimal shift).

    for (auto const& x : maxima) {
      fast_eafs.emplace_back(get_fast_eaf(x));
      auto const s = fast_eafs.back().k;
      if (s > shift)
        shift = s;
    }

    // Optimal shift is 2 * size since it prevents mshift to deal with partial
    // limbs. In addition, we subtract 1 to compensate shift's increment made
    // later on, when shift is output. (See below.)
    if (storage_base() == 10)
      shift = std::max(shift, 2 * size() - 1);

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
      "#define amaru_size                   " << size()                << "\n"
      "#define amaru_exponent_minimum       " << exponent_min()        << "\n"
      "#define amaru_mantissa_size          " << mantissa_size()       << "\n"
      "#define amaru_storage_base           " << storage_base()        << "\n"
      "#define amaru_storage_index_offset   " << index_offset()        << "\n";

    if (!calculation_div10().empty())
      stream << "#define amaru_calculation_div10      "
        "amaru_" << calculation_div10()   << "\n";

    stream <<
      "#define amaru_calculation_mshift     "
        "amaru_" << calculation_mshift() << "\n"
      // Instead of using mshift(m, upper, lower) / 2 in Amaru, shift is
      // incremented here and the division by 2 is removed.
      "#define amaru_calculation_shift      " << shift + 1             << "\n"
      "#define amaru_optimisation_integer   " << optimise_integer()    << "\n"
      "#define amaru_optimisation_mid_point " << optimise_midpoint()   << "\n"
        "\n"
      "#define amaru_function               " << function() << "\n"
      "#define amaru_fields_t               " << prefix() << "fields_t\n"
      "#define amaru_u1_t                   " << prefix() << "u1_t\n"
      "\n"
      "#if defined(" << prefix() << "u2_t)\n"
      "  #define amaru_u2_t                 " << prefix() << "u2_t\n"
      "#endif\n"
      "\n"
      "#if defined(" << prefix() << "u4_t)\n"
      "  #define amaru_u4_t                 " << prefix() << "u4_t\n"
      "#endif\n"
      "\n"
      "static struct {\n"
      "  amaru_u1_t const upper;\n"
      "  amaru_u1_t const lower;\n"
      "} const multipliers[] = {\n";

    auto e2      = exponent_min();
    auto e2_or_f = storage_base() == 10 ? amaru_log10_pow2(e2) : e2;

    splitter_t splitter{size(), storage_split()};

    for (const auto &fast_eaf : fast_eafs) {

      integer_t upper;
      integer_t lower;
      divide_qr(fast_eaf.U, p2_size, upper, lower);

      if (upper >= p2_size)
        throw exception_t{"Multiplier is out of range."};

      stream << "  { " << splitter(upper) << ", " << splitter(lower) <<
        " }, // " << std::dec << e2_or_f << "\n";

      ++e2_or_f;
    }

    stream << "};\n"
      "\n"
      "static struct {\n"
      "  amaru_u1_t const multiplier;\n"
      "  amaru_u1_t const bound;\n"
      "} const minverse[] = {\n";

    auto const minverse5 = integer_t{ p2_size - (p2_size - 1) / 5 };
    auto multiplier = integer_t{1};
    auto p5 = integer_t{1};

    // Amaru calls is_multiple_of_pow5(m, f) for
    //
    //   m =  2 * mantissa     - 1
    //   m =  2 * mantissa     + 1
    //   m =  4 * mantissa_min - 1
    //   m =  4 * mantissa_min * 2^{exponent - f} * 5^{-f}
    //     =  2 * mantissa_max * 2^{exponent - f} * 5^{-f}
    //   m = 40 * mantissa_min * 2^{exponent - f} * 5^{-f}
    //     = 20 * mantissa_max * 2^{exponent - f} * 5^{-f}
    //
    // where 2^{exponent} < 10^{f + 1}. Hence, 2^{exponent - f} < 5^f * 10
    // which yields 2^{exponent - f} * 5^{-f} < 10. Therefore,
    // 200 * mantissa_max is a conservative bound, i.e., if
    // 5^f > 200 * mantissa_max >= m, then is_multiple_of_pow5(m, f) == false;
    //
    // Also ensure that at least entries up to f = 1 are generated for
    // remove_trailing_zeros.
    for (std::int32_t f = 0; f < 1 || p5 <= 200 * mantissa_max(); ++f) {
      const auto bound = p2_size / p5 - (f == 0);

      stream << "  { " << splitter(multiplier) << ", " << splitter(bound) <<
        " },\n";

      multiplier = (multiplier * minverse5) % p2_size;
      p5 *= 5;
    }

    stream << std::dec <<
      "};\n"
      "\n"
      "#include \"amaru/amaru.h\"\n"
      "\n"
      "#ifdef __cplusplus\n"
      "}\n"
      "#endif\n";
  }

  /**
   * \brief Gets the maxima of all primary problems. (See get_maximum_primary.)
   *
   * It returns a vector v of size exponent_max() - exponent_min() + 1 such that
   * v[i] contains the maximum of the primary problem corresponding to exponent
   * = exponent_min() + i.
   *
   * \returns The vector v.
   */
  std::vector<alpha_delta_maximum_t>
  get_maxima() const {

    std::vector<alpha_delta_maximum_t> maxima;
    maxima.reserve(exponent_max() - exponent_min() + 1);

    auto f_done = amaru_log10_pow2(exponent_min()) - 1;

    for (auto e = exponent_min(); e <= exponent_max(); ++e) {

      auto const f = amaru_log10_pow2(e);

      if (storage_base() == 10 && f == f_done)
        continue;

      auto const e0 = (storage_base() == 10 ?
        e - amaru_log10_pow2_residual(e) : e) - f;

      alpha_delta_maximum_t x;
      x.alpha   = f >= 0 ? pow2(e0) : pow5(-f );
      x.delta   = f >= 0 ? pow5(f ) : pow2(-e0);
      x.maximum = get_maximum(x.alpha, x.delta, e == exponent_min());

      maxima.emplace_back(std::move(x));

      f_done = f;
    }
    return maxima;
  }

  /**
   * \brief Given alpha and delta, this function calculates the maximiser of
   * phi(m) over the relevant set of mantissas.
   *
   * \pre 0 <= alpha && 0 < delta.
   */
  rational_t
  get_maximum(integer_t alpha, integer_t const& delta,
    bool const start_at_1) const {

    alpha %= delta;

    // Usual interval.

    auto const a = start_at_1 ? integer_t{1} : integer_t{2 * mantissa_min()};
    auto const b = storage_base() == 10 ? integer_t{16 * mantissa_max() - 15} :
      integer_t{2 * mantissa_max()};

    auto const max_ab = get_maximum_1(alpha, delta, a, b);

    // Extras that are needed when mantissa == normal_mantissa_min().

    auto max_extras = [&](auto const& mantissa) {
      auto const m_a     =  4 * mantissa - 1;
      auto const m_c     = 20 * mantissa;
      auto const max_m_a = phi_1(alpha, delta, m_a);
      auto const max_m_c = phi_2(alpha, delta, m_c);
      return std::max(max_m_a, max_m_c);
    };

    if (storage_base() == 2)
      return std::max(max_ab, max_extras(mantissa_min()));

    return std::max({max_ab, max_extras(mantissa_min()),
      max_extras(2 * mantissa_min()), max_extras(4 * mantissa_min()),
      max_extras(8 * mantissa_min())});
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
  get_fast_eaf(alpha_delta_maximum_t const& x) const {

    // Making shift >= size, simplifies mshift executed at runtime. Indeed, it
    // ensures that the least significant limb of the product is irrelevant. For
    // this reason, later on, the generator actually outputs shift - size
    // (still labelling it as 'shift') so that Amaru doesn't need to do it at
    // runtime.
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

  generator_t const& self;

}; // generator_t::impl_t

//------------------------------------------------------------------------------
// generator_t
//------------------------------------------------------------------------------

generator_t::generator_t(config_t config, std::string directory) :
  config_      {std::move(config)                            },
  prefix_      {get_prefix(self().size())                    },
  function_    {"amaru_" + self().id()                       },
  mantissa_min_{amaru_pow2(integer_t, self().mantissa_size())},
  mantissa_max_{2 * self().mantissa_min()                    },
  index_offset_{get_index_offset(self().storage_base(),
    self().exponent_min())                                   },
  directory_   {std::move(directory)                         },
  dot_h_       {self().id() + ".h"                           },
  dot_c_       {self().id() + ".c"                           } {
}

void
generator_t::generate() const {

  auto const p2_size      = integer_t{1} << self().size();
  auto       dot_h_stream = std::ofstream{self().directory() +
    self().dot_h()};
  auto       dot_c_stream = std::ofstream{self().directory() +
    self().dot_c()};

  std::cout << "Generation started.\n";

  // Overflow check 1:
  if (2 * self().mantissa_max() + 1 >= p2_size)
    throw exception_t("The limb is not large enough for calculations to "
      "not overflow.");

  // Overflow check 2:
  if (20 * self().mantissa_min() >= p2_size)
    throw exception_t("The limb is not large enough for calculations to "
      "not overflow.");

  std::cout << "  Generating \"" << self().dot_h() << "\".\n";
  self().generate_dot_h(dot_h_stream);

  std::cout << "  Generating \"" << self().dot_c() << "\".\n";
  self().generate_dot_c(dot_c_stream);

  std::cout << "Generation finished.\n";
}

generator_t::impl_t const
generator_t::self() const {
  return impl_t{this};
}

} // namespace amaru
