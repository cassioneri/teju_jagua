// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2024 Cassio Neri <cassio.neri@gmail.com>

/**
 * @file cpp/common/exception.hpp
 *
 * Exception thrown by Teju Jagua.
 */

#ifndef TEJU_CPP_COMMON_EXCEPTION_HPP_
#define TEJU_CPP_COMMON_EXCEPTION_HPP_

#include <cstdio>
#include <stdexcept>

namespace teju {

/**
 * @brief Jeju Jagua's exception.
 */
struct exception_t : std::runtime_error {
  using std::runtime_error::runtime_error;
};

/**
 * @brief Throws an exception if a given condition doesn't hold.
 *
 * @param condition       The condition.
 * @param message         The error message.
 */
static inline
void
require(bool const condition, const char* const message) {
  if (!condition)
    throw exception_t{message};
}

/**
 * @brief Report an error to stderr.
 *
 * @param program         The program that generates the error, i.e., argv[0].
 * @param message         The error message.
 */
static inline
void
report_error(const char* const program, const char* const message) {
  std::fprintf(stderr, "%s: error: %s.\n", program, message);
}

} // namespace teju

#endif // TEJU_CPP_COMMON_EXCEPTION_HPP_
