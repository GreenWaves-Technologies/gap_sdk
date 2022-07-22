cmake_minimum_required(VERSION 3.14)

add_executable (backend_test test.cpp)
target_link_libraries (backend_test LINK_PUBLIC profiler_backend)