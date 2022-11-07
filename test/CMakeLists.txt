add_executable(test
  div10.cpp
  infimum.cpp
  log.cpp
  main.cpp
)

target_link_libraries(test
  amaru
  boost_multiprecision
  gtest gtest_main
  other
)
