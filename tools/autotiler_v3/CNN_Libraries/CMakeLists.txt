cmake_minimum_required(VERSION 3.16)

add_library(model_lib_pow2 OBJECT
    CNN_BiasReLULinear_BasicKernels.c
    CNN_Conv_BasicKernels.c
    CNN_Conv_DP_BasicKernels.c
    CNN_Conv_DW_BasicKernels.c
    CNN_Conv_DW_DP_BasicKernels.c
    CNN_Pooling_BasicKernels.c
    CNN_MatAlgebra.c
    CNN_SoftMax.c
    )

target_link_libraries(model_lib_pow2 PRIVATE freertos)

target_include_directories(model_lib_pow2 PRIVATE ${GWT_PMSIS_BSP}/include)

target_include_directories(model_lib_pow2 PUBLIC
    ${TILER_EMU_INC}
    ${TILER_CNN_KERNEL_PATH_FP16}
    .
    )
