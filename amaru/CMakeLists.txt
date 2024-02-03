add_library(amaru STATIC
  generated/ieee32.c
  generated/ieee64.c
  float.c
  double.c
)

if (AMARU_HAS_FLOAT128)
  target_sources(amaru PUBLIC
    generated/ieee128.c
    float128.c
  )
endif()

target_include_directories(amaru PUBLIC
  "${CMAKE_SOURCE_DIR}"
)
