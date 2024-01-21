/**
 * @file cpp/common/exception.hpp
 *
 * Exception thrown by Amaru.
 */

#ifndef AMARU_CPP_COMMON_EXCEPTION_HPP_
#define AMARU_CPP_COMMON_EXCEPTION_HPP_

#include <cstdio>
#include <stdexcept>

namespace amaru {

/**
 * @brief Amaru's exception.
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

} // namespace amaru

#endif // AMARU_CPP_COMMON_EXCEPTION_HPP_
