cmake_minimum_required(VERSION 3.16)

add_library(model_lib_sq8 OBJECT
    CNN_Activation_SQ8.c
    CNN_Activation_HWC_SQ8.c
    CNN_Bias_Linear_SQ8.c
    CNN_Conv_SQ8.c
    CNN_MatMul_Conv_SQ8.c
    CNN_Pooling_SQ8.c
    CNN_Conv_DW_SQ8.c
    CNN_MatAlgebra_SQ8.c
    CNN_SoftMax_SQ8.c
    RNN_SQ8.c
    )

target_link_libraries(model_lib_sq8 PRIVATE freertos)

target_include_directories(model_lib_sq8 PRIVATE ${GWT_PMSIS_BSP}/include)

target_include_directories(model_lib_sq8 PRIVATE
    ${TILER_EMU_INC}
    .
    )
