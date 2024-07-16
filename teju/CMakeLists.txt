# SPDX-License-Identifier: APACHE-2.0
# SPDX-FileCopyrightText: 2021-2024 Cassio Neri <cassio.neri@gmail.com>

add_library(teju STATIC
  generated/ieee16.c
  generated/ieee32.c
  float.c
  double.c
)

if (teju_has_uint128)
  target_sources(teju PUBLIC
    generated/ieee64_with_uint128.c
  )
else()
  target_sources(teju PUBLIC
    generated/ieee64_no_uint128.c
  )
endif()

if (teju_has_float128)
  target_sources(teju PUBLIC
    generated/ieee128.c
    float128.c
  )
endif()

target_include_directories(teju PUBLIC
  "${CMAKE_SOURCE_DIR}"
)
