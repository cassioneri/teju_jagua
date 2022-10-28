#include <cstdint>

namespace amaru {

/**
 * \brief Online mean and standard deviation calculator.
 *
 * WARN: Sample values must be small numbers of type std::uint64_t.
 */
struct stats_t {

  /**
   * \brief Updates mean and standard deviation after for a new sample
   * value.
   *
   * \param x The new sample value.
   */
  void
  update(std::uint64_t const x) {
    n_              += 1;
    sum_            += x;
    sum_of_squares_ += x * x;
  }

  /**
   * \brief Returns the current mean.
   */
  std::uint64_t
  mean() const {
    return sum_ / n_;
  }

  /**
   * \brief Returns the current standard deviation.
   */
  std::uint64_t
  stddev() const {
    auto const num = n_ * sum_of_squares_ - sum_ * sum_;
    auto const den = n_ * (n_ - 1);
    return std::sqrt(double(num) / double(den));
  }

private:
  std::uint64_t n_              = 0;
  std::uint64_t sum_            = 0;
  std::uint64_t sum_of_squares_ = 0;
};

} // namespace amaru
