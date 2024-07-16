// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2024 Cassio Neri <cassio.neri@gmail.com>

/**
 * @file cpp/test/synthetic_1.hpp
 *
 * Implementation teju_multiply used when testing synthetic_1 functions.
 */

#ifndef TEJU_CPP_TEST_SYNTHETIC_1_HPP_
#define TEJU_CPP_TEST_SYNTHETIC_1_HPP_

extern "C" {

static inline
teju_u1_t
teju_multiply(teju_u1_t const a, teju_u1_t const b, teju_u1_t* upper) {
  teju_u2_t const p = teju_u2_t(a) * teju_u2_t(b);
  *upper = teju_u1_t(p >> teju_size);
  return teju_u1_t(p);
}

}

#endif // TEJU_CPP_TEST_SYNTHETIC_1_HPP_
