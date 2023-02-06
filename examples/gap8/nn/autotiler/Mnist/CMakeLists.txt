cmake_minimum_required(VERSION 3.16)
include(${CONFIG_GAP_SDK_HOME}/utils/cmake/macros.cmake)
# set(CMAKE_VERBOSE_MAKEFILE ON)

setupcrosscompile()

set(TARGET_NAME "Mnist")
project(${TARGET_NAME} C ASM)

set(GWT_PMSIS_BSP ${CONFIG_GAP_SDK_HOME}/rtos/pmsis/bsp)

#===================================================================================================
# Paths
#===================================================================================================
if (NOT DEFINED TILER_PATH)
    if (NOT DEFINED ENV{TILER_PATH})
        set(TILER_PATH $ENV{TILER_PATH})
    else()
        set(TILER_PATH ${CONFIG_GAP_SDK_HOME}/tools/autotiler_v3)
    endif()
endif()

if (NOT DEFINED TILER_LIB)
    if (NOT DEFINED ENV{TILER_LIB})
        set(TILER_LIB $ENV{TILER_LIB})
    else()
        set(TILER_LIB ${CONFIG_GAP_SDK_HOME}/tools/autotiler_v3/Autotiler/LibTile.a)
    endif()
endif()

if (NOT DEFINED TILER_EMU_INC)
    if (NOT DEFINED ENV{TILER_EMU_INC})
        set(TILER_EMU_INC $ENV{TILER_EMU_INC})
    else()
        set(TILER_EMU_INC ${CONFIG_GAP_SDK_HOME}/tools/autotiler_v3/Emulation)
    endif()
endif()

if (NOT DEFINED TILER_INC)
    if (NOT DEFINED ENV{TILER_INC})
        set(TILER_INC $ENV{TILER_INC})
    else()
        set(TILER_INC ${CONFIG_GAP_SDK_HOME}/tools/autotiler_v3/Autotiler)
    endif()
endif()

#===================================================================================================
# Options
#===================================================================================================
set(MODEL_POW2 1)

#===================================================================================================
# Mnist
# - build steps:
#   [1] Build code generator GenMinst
#   [2] Run GenMinst on host to generate target sources (gen_kernels)
#   [3] Build target application Mnist
#===================================================================================================
add_executable(
    ${TARGET_NAME}
    $<TARGET_OBJECTS:cnn_lib>
    Mnist.c
    ${CMAKE_CURRENT_BINARY_DIR}/MnistKernels.c
    ImgIO.c
    )

target_compile_options(${TARGET_NAME} PRIVATE -fcommon -w -O3 -mno-memcpy -fno-tree-loop-distribute-patterns -fdata-sections -ffunction-sections -Wno-maybe-uninitialized -Wno-unused-but-set-variable)
target_link_options(${TARGET_NAME} PRIVATE -flto -Wl,--gc-sections)

setupos(${TARGET_NAME})

add_subdirectory(${TILER_PATH} ${CMAKE_CURRENT_BINARY_DIR}/cnn)

target_link_libraries(${TARGET_NAME} PRIVATE cnn_lib)

target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(${TARGET_NAME} PRIVATE ${GWT_PMSIS_BSP}/include)

#===================================================================================================
# GenMnist
# - custom command execution as it requires host compiler and is difficult
#   to use two toolchains within the same cmake project
#===================================================================================================
add_custom_target(
    GenMnist
    COMMAND gcc -o GenMnist -fcommon -I${TILER_INC} -I${TILER_EMU_INC} ${CNN_GEN_INCLUDE} ${CMAKE_CURRENT_SOURCE_DIR}/MnistModel.c ${CNN_GEN} ${TILER_LIB}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/MnistModel.c ${CNN_GEN} ${TILER_LIB}
    VERBATIM
    )

add_custom_target(
    gen_kernels
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/GenMnist
    DEPENDS GenMnist
    BYPRODUCTS MnistKernels.c
    VERBATIM
    )

add_dependencies(${TARGET_NAME} gen_kernels)
