cmake_minimum_required(VERSION 3.16)

add_library(model_lib_fp16 OBJECT
    CNN_Bias_Linear_Activation_fp16.c
    CNN_Conv_BasicKernels_fp16.c
    CNN_Conv_DW_BasicKernels_fp16.c
    CNN_MatMul_Conv_fp16.c
    CNN_MatAlgebra_fp16.c
    CNN_Pooling_BasicKernels_fp16.c
    CNN_SoftMax_fp16.c
    RNN_fp16.c
    )

target_link_libraries(model_lib_fp16 PRIVATE freertos)

target_include_directories(model_lib_fp16 PRIVATE ${GWT_PMSIS_BSP}/include)

target_include_directories(model_lib_fp16 PRIVATE
    ${TILER_EMU_INC}
    .
    )
