cmake_minimum_required(VERSION 3.16)

add_library(model_lib_ne16 OBJECT
    CNN_BasicKernels_NE16.c
    RNN_BasicKernels_NE16.c
    )

target_link_libraries(model_lib_ne16 PRIVATE freertos)

target_include_directories(model_lib_ne16 PRIVATE ${GWT_PMSIS_BSP}/include)

target_include_directories(model_lib_ne16 PUBLIC
    ${TILER_EMU_INC}
    .
    )
