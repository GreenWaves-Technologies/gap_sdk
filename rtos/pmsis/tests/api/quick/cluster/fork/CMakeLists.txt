cmake_minimum_required(VERSION 3.16)
include(${CONFIG_GAP_SDK_HOME}/utils/cmake/macros.cmake)

setupcrosscompile()

set(TARGET_NAME "test")
project(${TARGET_NAME} C ASM)
add_executable(${TARGET_NAME} test.c)

# add compiler options just for the test itself
target_compile_options(${TARGET_NAME} PRIVATE "-DNB_PE=8")

# include the OS
setupos(${OS_NAME} ${TARGET_NAME})
