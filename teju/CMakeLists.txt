# SPDX-License-Identifier: APACHE-2.0
# SPDX-FileCopyrightText: 2021-2025 Cassio Neri <cassio.neri@gmail.com>

add_library(teju STATIC)

#-------------------------------------------------------------------------------
# float
#-------------------------------------------------------------------------------

target_sources(teju PUBLIC float.c)

if (teju_has_uint128)
  target_sources(teju PUBLIC generated/ieee32_with_uint128.c)
else()
  target_sources(teju PUBLIC generated/ieee32_no_uint128.c)
endif()

#-------------------------------------------------------------------------------
# double
#-------------------------------------------------------------------------------

target_sources(teju PUBLIC double.c)

if (teju_has_uint128)
  target_sources(teju PUBLIC generated/ieee64_with_uint128.c)
else()
  target_sources(teju PUBLIC generated/ieee64_no_uint128.c)
endif()

#-------------------------------------------------------------------------------
# _Float16
#-------------------------------------------------------------------------------

if (teju_has_float16)

  target_sources(teju PUBLIC float16.c)

  # _Float16 can't work with uint16_t and needs uint32_t.
  if (teju_has_uint128)
    target_sources(teju PUBLIC generated/ieee16_with_uint128.c)
  else()
    target_sources(teju PUBLIC generated/ieee16_no_uint128.c)
  endif()

endif()

#-------------------------------------------------------------------------------
# _Float128
#-------------------------------------------------------------------------------

if (teju_has_float128)
  target_sources(teju PUBLIC
    float128.c
    generated/ieee128.c
  )
endif()

target_include_directories(teju PUBLIC
  "${CMAKE_SOURCE_DIR}"
)
