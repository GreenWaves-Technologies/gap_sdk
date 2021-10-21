set(TESTBENCH_SRCS "i2s_verif.cpp"
    "spim_verif.cpp"
    "testbench.cpp"
    )

vp_model(NAME testbench
    PREFIX "devices/testbench"
    SOURCES ${TESTBENCH_SRCS}
    )

find_library(SNDFILE_LIB sndfile)
if(SNDFILE_LIB)
    vp_model_compile_options(NAME testbench OPTIONS "-DUSE_SNDFILE")
    vp_model_link_libraries(NAME testbench LIBRARY sndfile)
endif()

