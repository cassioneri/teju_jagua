add_executable(generator
  generator.cpp
)

target_link_libraries(generator
  boost_multiprecision
  nlohmann_json::nlohmann_json
)

target_include_directories(generator PRIVATE "${CMAKE_SOURCE_DIR}/include")
