#-------------------------------------------------------------------------------
# generator
#-------------------------------------------------------------------------------

add_executable(generator
  generator.cpp
)

target_include_directories(generator PRIVATE "${CMAKE_SOURCE_DIR}/include")

target_link_libraries(generator
  boost_multiprecision
  nlohmann_json::nlohmann_json
)

#-------------------------------------------------------------------------------
# Amaru
#-------------------------------------------------------------------------------

add_library(amaru OBJECT
  ../generated/ieee32.c
  ../generated/ieee64.c
  double.c
  float.c
)

target_include_directories(amaru PUBLIC "${CMAKE_SOURCE_DIR}/include")

#-------------------------------------------------------------------------------
# test
#-------------------------------------------------------------------------------

add_executable(test
  test.cpp
)

#target_include_directories(test PRIVATE
#  "${CMAKE_SOURCE_DIR}/../"
#  "${CMAKE_SOURCE_DIR}/include"
#)

target_link_libraries(test
  amaru
  boost_multiprecision
  gtest gtest_main
)
