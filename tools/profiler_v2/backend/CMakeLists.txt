cmake_minimum_required(VERSION 3.14)

add_library (profiler_backend src/trace_parser.cpp src/trace.cpp src/hotspot.cpp)

target_include_directories (profiler_backend PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include $ENV{GVSOC_SRC_PATH}/engine/include)
target_link_directories (profiler_backend PUBLIC $ENV{INSTALL_DIR}/lib)
target_link_libraries (profiler_backend LINK_PUBLIC pthread pulpvp)