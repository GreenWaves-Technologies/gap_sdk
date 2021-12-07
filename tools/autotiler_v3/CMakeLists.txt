cmake_minimum_required(VERSION 3.16)

#===================================================================================================
# Paths
#===================================================================================================
if (NOT DEFINED TILER_CNN_KERNEL_PATH)
    if (NOT DEFINED ENV{TILER_CNN_KERNEL_PATH})
        set(TILER_CNN_KERNEL_PATH $ENV{TILER_CNN_KERNEL_PATH})
    else()
        set(TILER_CNN_KERNEL_PATH ${CONFIG_GAP_SDK_HOME}/tools/autotiler_v3/CNN_Libraries)
    endif()
endif()

if (NOT DEFINED TILER_DSP_KERNEL_PATH)
    if (NOT DEFINED ENV{TILER_DSP_KERNEL_PATH})
        set(TILER_DSP_KERNEL_PATH $ENV{TILER_DSP_KERNEL_PATH})
    else()
        set(TILER_DSP_KERNEL_PATH ${CONFIG_GAP_SDK_HOME}/tools/autotiler_v3/DSP_Libraries)
    endif()
endif()

if (NOT DEFINED TILER_DSP_GENERATOR_PATH)
    if (NOT DEFINED ENV{TILER_DSP_GENERATOR_PATH})
        set(TILER_DSP_GENERATOR_PATH $ENV{TILER_DSP_GENERATOR_PATH})
    else()
        set(TILER_DSP_GENERATOR_PATH ${CONFIG_GAP_SDK_HOME}/tools/autotiler_v3/DSP_Generators)
    endif()
endif()

if (NOT DEFINED TILER_BILINEAR_RESIZE_KERNEL_PATH)
    if (NOT DEFINED ENV{TILER_BILINEAR_RESIZE_KERNEL_PATH})
        set(TILER_BILINEAR_RESIZE_KERNEL_PATH $ENV{TILER_BILINEAR_RESIZE_KERNEL_PATH})
    else()
        set(TILER_BILINEAR_RESIZE_KERNEL_PATH ${CONFIG_GAP_SDK_HOME}/tools/autotiler_v3/Generators/BilinearResizes)
    endif()
endif()

if (NOT DEFINED TILER_BILINEAR_RESIZE_GENERATOR_PATH)
    if (NOT DEFINED ENV{TILER_BILINEAR_RESIZE_GENERATOR_PATH})
        set(TILER_BILINEAR_RESIZE_GENERATOR_PATH $ENV{TILER_BILINEAR_RESIZE_GENERATOR_PATH})
    else()
        set(TILER_BILINEAR_RESIZE_GENERATOR_PATH ${CONFIG_GAP_SDK_HOME}/tools/autotiler_v3/Generators/BilinearResizes)
    endif()
endif()

if (NOT DEFINED TILER_CNN_KERNEL_PATH_SQ8)
    if (NOT DEFINED ENV{TILER_CNN_KERNEL_PATH_SQ8})
        set(TILER_CNN_KERNEL_PATH_SQ8 $ENV{TILER_CNN_KERNEL_PATH_SQ8})
    else()
        set(TILER_CNN_KERNEL_PATH_SQ8 ${CONFIG_GAP_SDK_HOME}/tools/autotiler_v3/CNN_Libraries_SQ8)
    endif()
endif()

if (NOT DEFINED TILER_CNN_KERNEL_PATH_FP16)
    if (NOT DEFINED ENV{TILER_CNN_KERNEL_PATH_FP16})
        set(TILER_CNN_KERNEL_PATH_FP16 $ENV{TILER_CNN_KERNEL_PATH_FP16})
    else()
        set(TILER_CNN_KERNEL_PATH_FP16 ${CONFIG_GAP_SDK_HOME}/tools/autotiler_v3/CNN_Libraries_fp16)
    endif()
endif()

if (NOT DEFINED TILER_CNN_GENERATOR_PATH)
    if (NOT DEFINED ENV{TILER_CNN_GENERATOR_PATH})
        set(TILER_CNN_GENERATOR_PATH $ENV{TILER_CNN_GENERATOR_PATH})
    else()
        set(TILER_CNN_GENERATOR_PATH ${CONFIG_GAP_SDK_HOME}/tools/autotiler_v3/CNN_Generators)
    endif()
endif()

if (NOT DEFINED PMSIS_API)
    if (NOT DEFINED ENV{PMSIS_API})
        set(PMSIS_API $ENV{PMSIS_API})
    else()
        set(PMSIS_API ${CONFIG_GAP_SDK_HOME}/rtos/pmsis/pmsis_api)
    endif()
endif()



if (NOT DEFINED TILER_EMU_INC)
    if (NOT DEFINED ENV{TILER_EMU_INC})
        set(TILER_EMU_INC $ENV{TILER_EMU_INC})
    else()
        set(TILER_EMU_INC ${CONFIG_GAP_SDK_HOME}/tools/autotiler_v3/Emulation)
    endif()
endif()

#===================================================================================================
# tiler : a non-compiled library just to encapsulate commonly needed headers
#===================================================================================================
add_library(tiler INTERFACE)
target_include_directories(tiler INTERFACE
    Autotiler
    Emulation
    CNN_Libraries_fp16
    )

#===================================================================================================
# cnn_lib
#===================================================================================================
add_library(cnn_lib OBJECT
    ${TILER_CNN_KERNEL_PATH}/SSD_BasicKernels.c
    ${TILER_BILINEAR_RESIZE_KERNEL_PATH}/ResizeBasicKernels.c
    ${TILER_CNN_KERNEL_PATH}/CNN_Copy.c
    ${TILER_CNN_KERNEL_PATH_SQ8}/CNN_AT_Misc.c
    $<TARGET_OBJECTS:model_lib_dsp>
    )

target_link_libraries(cnn_lib PUBLIC tiler)
includeos(cnn_lib)
target_include_directories(cnn_lib PRIVATE ${GWT_PMSIS_BSP}/include)

add_subdirectory(${TILER_DSP_KERNEL_PATH} ${CMAKE_CURRENT_BINARY_DIR}/DSP_libraries)

#---------------------------------------------------------------------------------------------------
# cnn_lib: MODEL_POW2 option
if (MODEL_POW2 EQUAL 1)
    add_subdirectory(CNN_Libraries)
    target_link_libraries(cnn_lib PUBLIC model_lib_pow2)
    target_sources(cnn_lib PUBLIC
        $<TARGET_OBJECTS:model_lib_pow2>
        )
endif()

#---------------------------------------------------------------------------------------------------
# cnn_lib: MODEL_SQ8 option
if (MODEL_SQ8 EQUAL 1)
    add_subdirectory(CNN_Libraries_SQ8)
    target_link_libraries(cnn_lib PUBLIC model_lib_sq8)
    target_sources(cnn_lib PUBLIC
        $<TARGET_OBJECTS:model_lib_sq8>
        )
endif()

#---------------------------------------------------------------------------------------------------
# cnn_lib: MODEL_FP16 option
if (MODEL_FP16 EQUAL 1)
    add_subdirectory(CNN_Libraries_fp16)
    target_link_libraries(cnn_lib PUBLIC model_lib_fp16)
    target_sources(cnn_lib PUBLIC
        $<TARGET_OBJECTS:model_lib_fp16>
        )
endif()

#---------------------------------------------------------------------------------------------------
# cnn_lib: MODEL_NE16 option
if (MODEL_NE16 EQUAL 1)
    add_subdirectory(CNN_Libraries_NE16)
    target_link_libraries(cnn_lib PUBLIC model_lib_ne16)
    target_sources(cnn_lib PUBLIC
        $<TARGET_OBJECTS:model_lib_ne16>
        )
endif()

#===================================================================================================
# cnn_gen
# - not a library, just set of variables exposed to the parent scope
#===================================================================================================

# cnn_gen: Common sources/includes
set(CNN_GEN ${TILER_CNN_GENERATOR_PATH}/CNN_Generator_Util.c ${TILER_CNN_GENERATOR_PATH}/CNN_Copy_Generators.c ${TILER_CNN_GENERATOR_PATH}/SSD_Generators.c ${TILER_BILINEAR_RESIZE_GENERATOR_PATH}/ResizeGenerator.c  ${TILER_DSP_GENERATOR_PATH}/DSP_Generators.c)
set(CNN_GEN_INCLUDE -I${TILER_CNN_GENERATOR_PATH} -I${TILER_BILINEAR_RESIZE_GENERATOR_PATH} -I${TILER_CNN_KERNEL_PATH} -I${TILER_CNN_KERNEL_PATH_FP16} -I${TILER_DSP_GENERATOR_PATH})

# cnn_gen: Additional sources for POW2
if (MODEL_POW2 EQUAL 1)
    set(MODEL_GEN_POW2
        ${TILER_CNN_GENERATOR_PATH}/CNN_Generators.c
        )
    set(CNN_GEN ${CNN_GEN} ${MODEL_GEN_POW2})
    set(CNN_GEN_INCLUDE ${CNN_GEN_INCLUDE} -I${TILER_CNN_GENERATOR_PATH})
endif()

# cnn_gen: Additional sources for SQ8
if (MODEL_SQ8 EQUAL 1)
    set(MODEL_GEN_SQ8
        ${TILER_CNN_KERNEL_PATH_SQ8}/CNN_Generators_SQ8.c
        ${TILER_CNN_KERNEL_PATH_SQ8}/RNN_Generators_SQ8.c
        )
    set(CNN_GEN ${CNN_GEN} ${MODEL_GEN_SQ8})
    set(CNN_GEN_INCLUDE ${CNN_GEN_INCLUDE} -I${TILER_CNN_GENERATOR_PATH_SQ8})
endif()

# cnn_gen: Additional sources for NE16
if (MODEL_NE16 EQUAL 1)
    set(MODEL_GEN_NE16
        ${TILER_CNN_KERNEL_PATH_NE16}/CNN_Generators_NE16.c
        ${TILER_CNN_KERNEL_PATH_NE16}/RNN_Generators_NE16.c
        )
    set(CNN_GEN ${CNN_GEN} ${MODEL_GEN_NE16})
    set(CNN_GEN_INCLUDE ${CNN_GEN_INCLUDE} -I${TILER_CNN_GENERATOR_PATH_NE16})
endif()

# cnn_gen: Additional sources for FP16
if (MODEL_FP16 EQUAL 1)
    set(MODEL_GEN_FP16
        ${TILER_CNN_KERNEL_PATH_FP16}/CNN_Generators_fp16.c
        ${TILER_CNN_KERNEL_PATH_FP16}/RNN_Generators_fp16.c
        ${TILER_CNN_KERNEL_PATH_FP16}/SSD_Generators_fp16.c
        )
    set(CNN_GEN ${CNN_GEN} ${MODEL_GEN_FP16})
    set(CNN_GEN_INCLUDE ${CNN_GEN_INCLUDE} -I${TILER_CNN_GENERATOR_PATH_FP16})
endif()

set(CNN_GEN ${CNN_GEN} PARENT_SCOPE)
set(CNN_GEN_INCLUDE ${CNN_GEN_INCLUDE} PARENT_SCOPE)
