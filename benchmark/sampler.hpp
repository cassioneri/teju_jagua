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
 * \tparam T The floating point type corresponding to the mantissas.
 */
template <typename T>
struct random_provider_t {

  using traits_t = amaru::traits_t<T>;
  using limb_t   = typename traits_t::limb_t;

  random_provider_t(limb_t const n_mantissas) :
    n_mantissas_{n_mantissas} {
  }

  bool
  empty() const {
    return n_mantissas_ > 0;
  }

  /**
   * \brief Returns the next pseudo-random mantissa.
   */
  limb_t
  pop() {
    --n_mantissas_;
    return distribution_(device_);
  }

private:

  using distribution_t = std::uniform_int_distribution<limb_t>;

  static limb_t constexpr mantissa_max_ =
    AMARU_POW2(limb_t, traits_t::mantissa_size) - 1;

  limb_t          n_mantissas_;
  std::mt19937_64 device_;
  distribution_t  distribution_ = distribution_t{1, mantissa_max_};

}; // random_provider_t

/**
 * \brief A helper class that generates floating point numbers for usage in
 * tests and benchmarks.
 *
 * It gets mantissa values from a provider class and generates all values
 * with this mantissa. When these are exhausted, the provider is called
 * again and the cycle repeats.
 *
 * \tparam T The floating point number type.
 * \tparam P The mantissa provider type.
 */
template <typename T, typename P>
struct sampler_helper_t {

  using traits_t = amaru::traits_t<T>;
  using limb_t   = typename traits_t::limb_t;

  /**
   * \brief Constructor.
   *
   * \tparam provider The mantissa provider.
   */
  sampler_helper_t(P provider) :
    exponent_{0                  },
    mantissa_{provider.pop()     },
    provider_{std::move(provider)} {
  }

  bool
  empty() const {
    return !provider_.empty() || exponent_ < exponent_max_;
  }

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

  static constexpr limb_t exponent_max_ =
    AMARU_POW2(limb_t, traits_t::exponent_size) - 1;

  std::uint32_t exponent_;
  limb_t        mantissa_;
  P             provider_;

}; // sampler_helper_t

template <typename T, population_t population>
struct sampler_t;

template <typename T>
struct sampler_t<T, population_t::integer> {

  bool
  empty() const {
    return value_ > max_;
  }

  T
  pop() {
    return value_++;
  }

private:

  T value_{1};
  T max_  {65536};

}; // sampler_t<T, population_t::integer>

template <typename T>
struct sampler_t<T, population_t::mixed> {

  using traits_t = amaru::traits_t<T>;
  using limb_t   = typename traits_t::limb_t;

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

    explicit Provider(limb_t const n_mantissas) :
      empty_          {false},
      random_provider_{n_mantissas} {
    }

    bool
    empty() const {
      return empty_;
    }

    limb_t
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
