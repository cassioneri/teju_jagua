// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2024 Cassio Neri <cassio.neri@gmail.com>

/**
 * @file cpp/generator/multiprecision.hpp
 *
 * Multi-precision types used by the generator.
 */

#ifndef TEJU_CPP_GENERATOR_MULTIPRECISION_HPP_
#define TEJU_CPP_GENERATOR_MULTIPRECISION_HPP_

#include <boost/multiprecision/cpp_int.hpp>

namespace teju {

using integer_t  = boost::multiprecision::cpp_int;
using rational_t = boost::multiprecision::cpp_rational;

} // namespace teju

#endif // TEJU_CPP_GENERATOR_MULTIPRECISION_HPP_
