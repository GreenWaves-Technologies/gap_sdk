message(STATUS "TODO solve duplicate target \"i2s_microphone\"")
#dpi_model(NAME i2s_microphone
#    SOURCES "i2s_microphone.cpp"
#    )
#
#
#find_library(SNDFILE_LIB sndfile)
#if(SNDFILE_LIB)
#    target_compile_options(i2s_microphone PRIVATE "-DUSE_SNDFILE")
#    target_link_libraries(i2s_microphone PRIVATE sndfile)
#endif()
