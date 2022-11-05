cmake_minimum_required(VERSION 3.1)

#---------------------------------------------------------------------------
# amaru
#---------------------------------------------------------------------------

project(amaru
  VERSION 1.0
  DESCRIPTION "The Incan dragon-like creature which comes from the \
    underworld to help us efficiently finding decimal representations of \
    float point numbers."
  LANGUAGES C CXX
)

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_compile_options(-mpreferred-stack-boundary=6)
endif()

add_subdirectory(amaru)
add_subdirectory(benchmark)
add_subdirectory(generator)
add_subdirectory(other)
add_subdirectory(test)

include(FetchContent)

#---------------------------------------------------------------------------
# dragonbox
#---------------------------------------------------------------------------

FetchContent_Declare(
  dragonbox
  GIT_REPOSITORY https://github.com/jk-jeon/dragonbox.git
  GIT_TAG        1.1.3
  SOURCE_DIR     ${CMAKE_SOURCE_DIR}/tp/dragonbox
)

set(DRAGONBOX_INSTALL_TO_CHARS off)
FetchContent_MakeAvailable(dragonbox)

#---------------------------------------------------------------------------
# googletest
#---------------------------------------------------------------------------

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.12.1
  SOURCE_DIR     ${CMAKE_SOURCE_DIR}/tp/googletest
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

#---------------------------------------------------------------------------
# json
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

#---------------------------------------------------------------------------
# multiprecision
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
