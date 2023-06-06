add_executable(test
  log.cpp
  main.cpp
  mshift.cpp
)

target_link_libraries(test
  amaru
  boost_multiprecision
  gtest gtest_main
  other
)

add_subdirectory(div10)
