// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

#include "cpp/test/impl.hpp"

// Other macros required by teju/div10.h and teju/mshift.h.
#define teju_do_not_define_teju_multiply
#define teju_calculation_mshift teju_synthetic_1
#define teju_calculation_shift  32

#include "cpp/test/synthetic_1.hpp"

#include "teju/div10.h"
#include "teju/mshift.h"

using impl_t = teju::test::synthetic_1_t;

teju_u1_t
impl_t::div10(teju_u1_t const m) {
  return teju_div10(m);
}

teju_u1_t
impl_t::mshift(teju_u1_t const m, teju_multiplier_t const M) {
  return teju_mshift(m, M);
}
