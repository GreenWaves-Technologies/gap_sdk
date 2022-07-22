set(NE16_SRCS
    "src/fsm.cpp"
    "src/ne16.cpp"
    "src/ne16_clear.cpp"
    "src/ne16_debug.cpp"
    "src/ne16_index.cpp"
    "src/ne16_load.cpp"
    "src/ne16_matrixvec.cpp"
    "src/ne16_normquant.cpp"
    "src/ne16_regfile.cpp"
    "src/ne16_streamin.cpp"
    "src/ne16_streamout.cpp"
    "src/stream.cpp"
    )
vp_model(NAME ne16
    PREFIX "pulp/ne16"
    SOURCES ${NE16_SRCS}
    )

vp_model_link_options(NAME ne16 OPTIONS -fopenmp)

vp_model_include_directories(
    NAME ne16
    DIRECTORY "include"
    )

vp_model_compile_options(NAME ne16 OPTIONS -fopenmp)
