add_executable(generator
  main.cpp
)

target_include_directories(generator PRIVATE
  "${CMAKE_SOURCE_DIR}"
)

target_link_libraries(generator PRIVATE
  boost_multiprecision
  nlohmann_json::nlohmann_json
)
