cmake_minimum_required(VERSION 3.1)

project(amaru
  VERSION 1.0
  DESCRIPTION "The Incan dragon-like creature from the underworld which \
    efficiently finds decimal representations of float point numbers."
  LANGUAGES C CXX
)

include(FetchContent)

#---------------------------------------------------------------------------
# Boost Multiprecision
#---------------------------------------------------------------------------

FetchContent_Declare(
  multiprecision
  GIT_REPOSITORY https://github.com/boostorg/multiprecision.git
  GIT_TAG        Boost_1_80_0
  SOURCE_DIR     ${CMAKE_SOURCE_DIR}/tp/multiprecision
)

set(BOOST_MP_STANDALONE on)
set(BUILD_TESTING off)
FetchContent_MakeAvailable(multiprecision)

#---------------------------------------------------------------------------
# nlohmann JSON
#---------------------------------------------------------------------------

FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG        v3.11.2
  SOURCE_DIR     ${CMAKE_SOURCE_DIR}/tp/json
)

set(BOOST_MP_STANDALONE on)
set(BUILD_TESTING off)
FetchContent_MakeAvailable(json)

add_subdirectory(src)
