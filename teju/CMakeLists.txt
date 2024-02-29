# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2024 Cassio Neri <cassio.neri@gmail.com>

add_library(teju STATIC
  generated/ieee32.c
  float.c
  double.c
)

if (TEJU_HAS_UINT128)
  target_sources(teju PUBLIC
    generated/ieee64_with_uint128.c
  )
else()
  target_sources(teju PUBLIC
    generated/ieee64_no_uint128.c
  )
endif()

if (TEJU_HAS_FLOAT128)
  target_sources(teju PUBLIC
    generated/ieee128.c
    float128.c
  )
endif()

target_include_directories(teju PUBLIC
  "${CMAKE_SOURCE_DIR}"
)
