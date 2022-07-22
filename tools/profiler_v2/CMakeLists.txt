cmake_minimum_required(VERSION 3.14)

add_subdirectory (backend)
#add_subdirectory (tests/backend)
#add_subdirectory (tests/gui2)
add_subdirectory (gui)
#add_subdirectory (tests/gui3)


install(TARGETS profiler DESTINATION bin)
