# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2024 Cassio Neri <cassio.neri@gmail.com>

#-------------------------------------------------------------------------------
# cmake
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.5)

project(teju_jagua
  VERSION 1.0
  DESCRIPTION "Teju Jagua: the gentle 7-headed beast from Guarani mythology \
    that comes out of its cave to help us efficiently finding decimal       \
    representations of floating-point numbers."
  LANGUAGES C CXX
)

cmake_policy(SET CMP0076 NEW)

set(CMAKE_C_STANDARD   99)
set(CMAKE_CXX_STANDARD 17)

# Sets optimisation level to -O3 in RelWithDebInfo build for GNU and Clang.

if (CMAKE_C_COMPILER_ID STREQUAL "GNU|Clang" AND NOT MSVC)
  foreach (flags IN ITEMS CMAKE_C_FLAGS_RELWITHDEBINFO
    CMAKE_CXX_FLAGS_RELWITHDEBINFO)

    string(REPLACE "-O2" "-O3" ${flags} ${${flags}})

  endforeach()
endif()

if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")

  # MSVC's C++ compiler seems to emit better code than its C counterparty hence,
  # we specify that all source files are C++.
  add_compile_options($<$<COMPILE_LANGUAGE:C>:/TP>)

  # Sets inlining level to /Ob3 in Release and RelWithDebInfo builds for MSVC
  # compilers.

  foreach (flags IN ITEMS CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_RELWITHDEBINFO
    CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_RELWITHDEBINFO)

    string(REGEX REPLACE "/Ob[0-9]" "/Ob3" ${flags} ${${flags}})

  endforeach()
endif()

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

# Tests existence of __uint128_t.

try_compile(teju_has_uint128
  ${CMAKE_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}/cmake/uint128.c
)

if (teju_has_uint128)
  add_compile_definitions(teju_has_uint128)
endif()

# Tests existence of __float128.

try_compile(teju_has_float128
  ${CMAKE_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}/cmake/float128.c
)

if (teju_has_float128)
  add_compile_definitions(teju_has_float128)
endif()

set(third_party_dir ${CMAKE_SOURCE_DIR}/third-party)

include(FetchContent)

#-------------------------------------------------------------------------------
# teju_jagua
#-------------------------------------------------------------------------------

add_subdirectory(teju)
add_subdirectory(cpp)

#-------------------------------------------------------------------------------
# dragonbox
#-------------------------------------------------------------------------------

FetchContent_Declare(
  dragonbox
  URL https://github.com/jk-jeon/dragonbox/archive/refs/tags/1.1.3.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  SOURCE_DIR ${third_party_dir}/dragonbox
)

set(DRAGONBOX_INSTALL_TO_CHARS off)
FetchContent_MakeAvailable(dragonbox)

#-------------------------------------------------------------------------------
# googletest
#-------------------------------------------------------------------------------

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  SOURCE_DIR ${third_party_dir}/googletest
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

#-------------------------------------------------------------------------------
# json
#-------------------------------------------------------------------------------

FetchContent_Declare(
  json
  URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  SOURCE_DIR ${third_party_dir}/json
)

FetchContent_MakeAvailable(json)

#-------------------------------------------------------------------------------
# multiprecision
#-------------------------------------------------------------------------------

FetchContent_Declare(
  multiprecision
  URL https://github.com/boostorg/multiprecision/archive/refs/tags/Boost_1_84_0.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  SOURCE_DIR     ${third_party_dir}/multiprecision
)

set(BOOST_MP_STANDALONE on)
set(BUILD_TESTING off)
FetchContent_MakeAvailable(multiprecision)

#-------------------------------------------------------------------------------
# nanobench
#-------------------------------------------------------------------------------

FetchContent_Declare(
  nanobench
  URL https://github.com/martinus/nanobench/archive/refs/tags/v4.3.11.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  SOURCE_DIR ${third_party_dir}/nanobench
)

FetchContent_MakeAvailable(nanobench)

#-------------------------------------------------------------------------------
# ryu
#-------------------------------------------------------------------------------

FetchContent_Declare(
  ryu
  URL https://github.com/cassioneri/ryu-non-official/archive/refs/tags/v1.0.0.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
  SOURCE_DIR ${third_party_dir}/ryu
)

FetchContent_MakeAvailable(ryu)
