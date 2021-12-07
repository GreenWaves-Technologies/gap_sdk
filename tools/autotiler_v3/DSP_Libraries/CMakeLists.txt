cmake_minimum_required(VERSION 3.16)

add_library(model_lib_dsp OBJECT
    CmplxFunctions.c
    FFT_Library.c
    MfccBasicKernels.c
    PreProcessing.c
    math_funcs.c 
    pulp_dsp/plp_cos_f32s_xpulpv2.c
    pulp_dsp/plp_sin_f32s_xpulpv2.c 
    pulp_dsp/plp_common_tables.c
    )

target_link_libraries(model_lib_dsp PRIVATE freertos)

target_include_directories(model_lib_dsp PRIVATE ${GWT_PMSIS_BSP}/include)

target_include_directories(model_lib_dsp PUBLIC
    ${TILER_EMU_INC}
    .
    )
