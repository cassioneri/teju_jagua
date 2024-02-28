// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2024 Cassio Neri <cassio.neri@gmail.com>

/**
 * @file cpp/generator/multiprecision.hpp
 *
 * Multi-precision types used by the generator.
 */

#ifndef AMARU_CPP_GENERATOR_MULTIPRECISION_HPP_
#define AMARU_CPP_GENERATOR_MULTIPRECISION_HPP_

#include <boost/multiprecision/cpp_int.hpp>

namespace amaru {

using integer_t  = boost::multiprecision::cpp_int;
using rational_t = boost::multiprecision::cpp_rational;

} // namespace amaru

#endif // AMARU_CPP_GENERATOR_MULTIPRECISION_HPP_
