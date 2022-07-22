cmake_minimum_required(VERSION 3.13)

set(CONFIG_USE_BSP 1)

if(NOT CONFIG_GAP_SDK_HOME)
    set(CONFIG_GAP_SDK_HOME $ENV{GAP_SDK_HOME})
endif()

include($ENV{GAP_SDK_HOME}/utils/cmake/setup.cmake)
setup_cmake()

set(TARGET_NAME "test")
project(${TARGET_NAME} C ASM)
add_executable(${TARGET_NAME} test.c)

message("BOARD NAME=${BOARD}")

# add compiler options just for the test itself
if(DEFINED RAM_TYPE)
    set(RAM_TYPE ${RAM_TYPE})
else()
    set(RAM_TYPE HYPERRAM)
endif()

message ("RAM_TYPE=${RAM_TYPE}")
target_compile_options(${TARGET_NAME} PRIVATE "-DUSE_${RAM_TYPE} -DASYNC")

# include the OS
setupos(${TARGET_NAME})
