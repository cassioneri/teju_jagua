// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

/**
 * @file cpp/test/synthetic_2.hpp
 *
 * Implementation teju_multiply used when testing synthetic_2 functions.
 */

#ifndef TEJU_CPP_TEST_SYNTHETIC_2_HPP_
#define TEJU_CPP_TEST_SYNTHETIC_2_HPP_

extern "C" {

static inline
teju_u2_t
teju_multiply(teju_u2_t const a, teju_u2_t const b, teju_u2_t* upper) {
  teju_u4_t const p = teju_u4_t(a) * teju_u4_t(b);
  *upper = teju_u2_t(p >> (2 * teju_width));
  return teju_u2_t(p);
}

}

#endif // TEJU_CPP_TEST_SYNTHETIC_2_HPP_
