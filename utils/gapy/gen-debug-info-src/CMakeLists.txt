cmake_minimum_required(VERSION 3.16.3)

project(gen-debug-info)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/ext
    ${CMAKE_CURRENT_BINARY_DIR}
    )

link_directories(${CMAKE_SOURCE_DIR}/ext)

add_executable(
    gen-debug-info
    main.cpp
    )

target_link_libraries(gen-debug-info bfd iberty dl z)

install(TARGETS gen-debug-info DESTINATION bin)
