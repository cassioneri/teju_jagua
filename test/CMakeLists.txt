add_executable(test

  # Tests
  div10.cpp
  log.cpp
  main.cpp
  mshift.cpp

  # Implementations of div10 and mshift
  built_in_1.cpp
  built_in_2.cpp
  built_in_4.cpp
  syntectic_1.cpp
  syntectic_1_small_shift.cpp
  syntectic_2.cpp
  syntectic_2_small_shift.cpp
)

target_link_libraries(test
  amaru
  boost_multiprecision
  gtest gtest_main
  other
)
