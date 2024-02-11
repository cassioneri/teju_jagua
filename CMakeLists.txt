#-------------------------------------------------------------------------------
# cmake
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.5)

project(amaru
  VERSION 1.0
  DESCRIPTION "The Incan dragon-like creature which comes from the underworld \
    to help us efficiently finding decimal representations of floating-point  \
    numbers."
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

try_compile(AMARU_HAS_UINT128
  ${CMAKE_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}/cmake/uint128.c
)

if (AMARU_HAS_UINT128)
  add_compile_definitions(AMARU_HAS_UINT128)
endif()

# Tests existence of __float128.

try_compile(AMARU_HAS_FLOAT128
  ${CMAKE_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}/cmake/float128.c
)

if (AMARU_HAS_FLOAT128)
  add_compile_definitions(AMARU_HAS_FLOAT128)
endif()

set(third_party_dir ${CMAKE_SOURCE_DIR}/third-party)

include(FetchContent)

#-------------------------------------------------------------------------------
# amaru
#-------------------------------------------------------------------------------

add_subdirectory(amaru)
add_subdirectory(cpp)

#-------------------------------------------------------------------------------
# dragonbox
#-------------------------------------------------------------------------------

FetchContent_Declare(
  dragonbox
  GIT_REPOSITORY https://github.com/jk-jeon/dragonbox.git
  GIT_TAG        1.1.3
  GIT_SHALLOW    TRUE
  SOURCE_DIR     ${third_party_dir}/dragonbox
)

set(DRAGONBOX_INSTALL_TO_CHARS off)
FetchContent_MakeAvailable(dragonbox)

#-------------------------------------------------------------------------------
# googletest
#-------------------------------------------------------------------------------

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.12.1
  GIT_SHALLOW    TRUE
  SOURCE_DIR     ${third_party_dir}/googletest
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

#-------------------------------------------------------------------------------
# json
#-------------------------------------------------------------------------------

FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG        v3.11.3
  GIT_SHALLOW    TRUE
  SOURCE_DIR     ${third_party_dir}/json
)

set(BOOST_MP_STANDALONE on)
set(BUILD_TESTING off)
FetchContent_MakeAvailable(json)

#-------------------------------------------------------------------------------
# multiprecision
#-------------------------------------------------------------------------------

FetchContent_Declare(
  multiprecision
  GIT_REPOSITORY https://github.com/boostorg/multiprecision.git
  GIT_TAG        Boost_1_80_0
  GIT_SHALLOW    TRUE
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
  GIT_REPOSITORY https://github.com/martinus/nanobench.git
  GIT_TAG        v4.3.11
  GIT_SHALLOW    TRUE
  SOURCE_DIR     ${third_party_dir}/nanobench
)
FetchContent_MakeAvailable(nanobench)
