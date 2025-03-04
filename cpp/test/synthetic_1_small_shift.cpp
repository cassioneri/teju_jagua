// SPDX-License-Identifier: APACHE-2.0
// SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

#include "cpp/test/impl.hpp"

// Other macros required by teju/mshift.h.
#define teju_do_not_define_teju_multiply
#define teju_calculation_mshift teju_synthetic_1
#define teju_calculation_shift  24

#include "cpp/test/synthetic_1.hpp"

#include "teju/mshift.h"

using impl_t = teju::test::synthetic_1_small_shift_t;

int const impl_t::shift = teju_calculation_shift;

teju_u1_t
impl_t::mshift(teju_calc_t const m, teju_u1_t const u, teju_u1_t const l) {
  return teju_mshift(m, u, l);
}
