cmake_minimum_required(VERSION 3.5)
project(pulp-tools VERSION 1.0.0)
include(ExternalProject)

option(WithFrameBuffer "Build with framebuffer support" OFF)
option(WithFTDI "Build with FTDI support" ON)
option(WithConfigGenerators "Install the config tools" ON)
option(WithFlasher "Build the flasher" OFF)
option(WithTests "Build tests" OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo"
        CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
        FORCE
    )
endif(NOT CMAKE_BUILD_TYPE)

macro(add_project name)
    ExternalProject_Add(
        ${name}
        SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/${name}
        DOWNLOAD_COMMAND ""
        LOG_DOWNLOAD ON
        BUILD_ALWAYS TRUE
        CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_IGNORE_PATH=${CMAKE_IGNORE_PATH} ${ARGN}
    )
endmacro()

add_project(json-tools)
add_project(pulp-configs -DWithConfigGenerators=${WithConfigGenerators})

if(DEFINED TARGET_INSTALL_DIR AND TARGET_INSTALL_DIR)
    get_filename_component(ABS_TARGET_INSTALL_DIR ${TARGET_INSTALL_DIR} ABSOLUTE BASE_DIR ${CMAKE_CURRENT_LIST_DIR})
    message(STATUS "TARGET_INSTALL_DIR is defined: ${ABS_TARGET_INSTALL_DIR}")
    add_project(pulp-debug-bridge -DWithFrameBuffer=${WithFrameBuffer} -DWithFTDI=${WithFTDI} -DWithTests=${WithTests} -DTARGET_INSTALL_DIR=${ABS_TARGET_INSTALL_DIR})
    if(WithFlasher)
        add_project(gap_flasher -DTARGET_INSTALL_DIR=${ABS_TARGET_INSTALL_DIR} -DCMAKE_TOOLCHAIN_FILE=toolchain/riscv32-unknown-elf-gap8.cmake)
    endif()
else()
    message(STATUS "TARGET_INSTALL_DIR is not defined")
    add_project(pulp-debug-bridge -DWithFrameBuffer=${WithFrameBuffer} -DWithFTDI=${WithFTDI} -DWithTests=${WithTests})
    if(WithFlasher)
        add_project(gap_flasher -DCMAKE_TOOLCHAIN_FILE=toolchain/riscv32-unknown-elf-gap8.cmake)
    endif()
endif()

add_custom_target(pulp-tools ALL)

add_dependencies(pulp-configs json-tools)
add_dependencies(pulp-debug-bridge pulp-configs)
add_dependencies(pulp-tools pulp-debug-bridge)
if(WithFlasher)
    add_dependencies(pulp-tools gap_flasher)
endif()

install(CODE "")