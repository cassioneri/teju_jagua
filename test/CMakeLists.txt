add_executable(test
  main.cpp
)

#target_include_directories(test PRIVATE
#  "${CMAKE_SOURCE_DIR}/../"
#  "${CMAKE_SOURCE_DIR}/include"
#)

target_link_libraries(test
  amaru
  boost_multiprecision
  gtest gtest_main
  other
)
