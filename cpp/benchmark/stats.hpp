// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2024 Cassio Neri <cassio.neri@gmail.com>

/**
 * @file cpp/benchmark/stats.hpp
 *
 * Statistic calculators.
 */

#ifndef TEJU_CPP_BENCHMARK_STATS_HPP_
#define TEJU_CPP_BENCHMARK_STATS_HPP_

#include <cmath>
#include <cstdint>

namespace teju {

/**
 * @brief Online mean and standard deviation calculator.
 *
 * @warning Sample values must be small numbers of type std::uint64_t.
 */
struct stats_t {

  /**
   * @brief Updates mean and standard deviation given a new sample value.
   *
   * @param x The new sample value.
   */
  void
  update(std::uint64_t const x) {
    n_              += 1;
    sum_            += x;
    sum_of_squares_ += x * x;
  }

  /**
   * @brief Gets the current mean.
   *
   * @returns The current mean.
   */
  std::uint64_t
  mean() const {
    return (sum_ + n_ / 2) / n_;
  }

  /**
   * @brief Gets the current standard deviation.
   *
   * @returns The current standard deviation.
   */
  std::uint64_t
  stddev() const {
    auto const num = n_ * sum_of_squares_ - sum_ * sum_;
    auto const den = n_ * (n_ - 1);
    return std::uint64_t(std::sqrt(double(num) / double(den)));
  }

private:
  std::uint64_t n_              = 0;
  std::uint64_t sum_            = 0;
  std::uint64_t sum_of_squares_ = 0;
};

} // namespace teju

#endif // TEJU_CPP_BENCHMARK_STATS_HPP_
