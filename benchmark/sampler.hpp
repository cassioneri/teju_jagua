#include "benchmark/traits.hpp"

#include <cstdint>
#include <random>

namespace amaru {

/**
 * \brief Types of population used in benchmarks.
 */
enum class population_t {

  // All integers in the interval [1, N[ for some N.
  integer,

  // Random numbers that are equidistant from its neighbours.
  centred,

  // All numbers that are not equidistant from its neighbours.
  uncentred,

  // Mix of centred and uncentred.
  mixed
};

/**
 * \brief Draws uniformly distributed pseudo-random mantissas.
 *
 * \tparam T                The floating point type corresponding to the
 *                          mantissa.
 */
template <typename T>
struct random_provider_t {

  using traits_t = amaru::traits_t<T>;
  using u1_t     = typename traits_t::u1_t;

  /**
   * \brief Constructor.
   *
   * \param n_mantissas     The number of mantissas to be generated.
   */
  random_provider_t(u1_t const n_mantissas) :
    n_mantissas_{n_mantissas} {
  }

  bool
  empty() const {
    return n_mantissas_ == 0;
  }

  /**
   * \brief Returns the next pseudo-random mantissa.
   */
  u1_t
  pop() {
    --n_mantissas_;
    return distribution_(device_);
  }

private:

  using distribution_t = std::uniform_int_distribution<u1_t>;

  static u1_t constexpr mantissa_max_ =
    amaru_pow2(u1_t, traits_t::mantissa_size) - 1;

  u1_t            n_mantissas_;
  std::mt19937_64 device_;
  distribution_t  distribution_ = distribution_t{1, mantissa_max_};

}; // random_provider_t

/**
 * \brief A helper class that generates floating point numbers for usage in
 * tests and benchmarks.
 *
 * It gets a mantissa value from a provider class and loops over the set of
 * exponents, generating all floating point values with the obtained
 * mantissa. When the exponents are exhausted, the provider is called again
 * and the cycle repeats.
 *
 * \tparam T The floating point number type.
 * \tparam P The mantissa provider type.
 */
template <typename T, typename P>
struct sampler_helper_t {

  using traits_t = amaru::traits_t<T>;
  using u1_t     = typename traits_t::u1_t;

  /**
   * \brief Constructor.
   *
   * \tparam provider       The mantissa provider.
   */
  sampler_helper_t(P provider) :
    exponent_{0                  },
    mantissa_{provider.pop()     },
    provider_{std::move(provider)} {
  }

  /**
   * \brief Tells whether there is still a value to be generated.
   */
  bool
  empty() const {
    return exponent_ == exponent_max_;
  }

  /**
   * \brief Gets the next floating point number.
   *
   * \pre !empty()
   */
  T
  pop() {

    auto const value = traits_t::from_ieee(exponent_, mantissa_);
    ++exponent_;

    // Avoids exponent == exponent_max_, since the corresponding value is
    // infinity or NaN.
    if (!provider_.empty() && exponent_ == exponent_max_) {
      exponent_ = 0;
      mantissa_ = provider_.pop();
    }

    return value;
  }

private:

  static constexpr u1_t exponent_max_ = amaru_pow2(u1_t,
    traits_t::exponent_size) - 1;

  u1_t exponent_;
  u1_t mantissa_;
  P    provider_;

}; // sampler_helper_t

/**
 * \brief Generates floating point numbers for usage in tests and
 * benchmarks.
 *
 * \tparam T                The floating point number type.
 * \tparam population       The type of population.
 */
template <typename T, population_t population>
struct sampler_t;

// Specialisation for population_t::integer.
template <typename T>
struct sampler_t<T, population_t::integer> {

  /**
   * \brief Tells whether there is still a value to be generated.
   */
  bool
  empty() const {
    return value_ >= max_;
  }

  /**
   * \brief Gets the next floating point number.
   *
   * \pre !empty()
   */
  T
  pop() {
    return value_++;
  }

private:

  T value_{1};
  T max_  {100000};

}; // sampler_t<T, population_t::integer>

// Specialisation for population_t::centred.
template <typename T>
struct sampler_t<T, population_t::centred> {

  using traits_t = amaru::traits_t<T>;
  using u1_t     = typename traits_t::u1_t;

  /**
   * \brief Tells whether there is still a value to be generated.
   */
  bool
  empty() const {
    return helper_.empty();
  }

  /**
   * \brief Gets the next floating point number.
   *
   * \pre !empty()
   */
  T
  pop() {
    return helper_.pop();
  }

private:

  using provider_t = random_provider_t<T>;
  sampler_helper_t<T, provider_t> helper_{provider_t{256}};

}; // sampler_t<T, population_t::centred>

// Specialisation for population_t::uncentred.
template <typename T>
struct sampler_t<T, population_t::uncentred> {

  using traits_t = amaru::traits_t<T>;
  using u1_t   = typename traits_t::u1_t;

  /**
   * \brief Tells whether there is still a value to be generated.
   */
  bool
  empty() const {
    return helper_.empty();
  }

  /**
   * \brief Gets the next floating point number.
   *
   * \pre !empty()
   */
  T
  pop() {
    return helper_.pop();
  }

private:

  struct provider_t {

    /**
     * \brief Tells whether there is still a value to be generated.
     */
    bool
    empty() const {
      return empty_;
    }

    /**
     * \brief Gets the next floating point number.
     *
     * \pre !empty()
     */
    u1_t
    pop() {
      empty_ = true;
      return 0;
    }

  private:
    bool empty_{false};
  };

  sampler_helper_t<T, provider_t> helper_{provider_t{}};

}; // sampler_t<T, population_t::uncentred>

// Specialisation for population_t::mixed.
template <typename T>
struct sampler_t<T, population_t::mixed> {

  using traits_t = amaru::traits_t<T>;
  using u1_t     = typename traits_t::u1_t;

  bool
  empty() const {
    return helper_.empty();
  }

  T
  pop() {
    return helper_.pop();
  }

private:

  struct Provider {

    explicit Provider(u1_t const n_mantissas) :
      empty_          {false},
      random_provider_{n_mantissas} {
    }

    bool
    empty() const {
      return empty_;
    }

    u1_t
    pop() {
      empty_ = random_provider_.empty();
      return !empty_ ? random_provider_.pop() : 0;
    }

  private:
    bool                 empty_;
    random_provider_t<T> random_provider_;
  };

  sampler_helper_t<T, Provider> helper_{Provider{255}};

}; // sampler_t<T, population_t::mixed>

} // namespace amaru
