cmake_minimum_required(VERSION 3.10)

add_library(gvsoc_launcher_lib SHARED "src/launcher.cpp")
target_include_directories(gvsoc_launcher_lib PUBLIC "include/")
target_link_libraries(gvsoc_launcher_lib PRIVATE pthread)
