file(
  GLOB_RECURSE SOURCES
  "Source/*.cpp"
  "Source/*.hpp"
  "Source/*.h"
  "Source/*.rc"
  "Source/main.cpp"
)

add_executable(
  Rose WIN32 ${SOURCES}
)

target_compile_features(Rose PRIVATE cxx_std_17)

target_include_directories(
  Rose PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/External
  ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

target_link_libraries(
  Rose PRIVATE 
  d3d12
  dxgi
  d3dcompiler
)