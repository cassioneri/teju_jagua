add_library(amaru STATIC
  generated/ieee32.c
  generated/ieee64.c
  double.c
  float.c
)

target_include_directories(amaru PUBLIC
  "${CMAKE_SOURCE_DIR}"
)
