add_library(amaru STATIC
  generated/ieee32.c
  generated/ieee64.c
  float.c
  double.c
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_sources(amaru PUBLIC
    generated/ieee128.c
    float128.c
  )
endif()

target_include_directories(amaru PUBLIC
  "${CMAKE_SOURCE_DIR}"
)
