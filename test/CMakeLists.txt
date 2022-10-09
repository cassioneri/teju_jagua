add_executable(test
  main.cpp
)

target_link_libraries(test
  amaru
  boost_multiprecision
  gtest gtest_main
  other
)
