add_subdirectory(dac)

set(SOUND_PREFIX "devices/sound")

vp_model(NAME i2s_microphone
         PREFIX ${SOUND_PREFIX}
         SOURCES "i2s_microphone.cpp")

vp_model(NAME i2s_speaker
         PREFIX ${SOUND_PREFIX}
         SOURCES "i2s_speaker.cpp")

vp_model(NAME i2s_clock
         PREFIX ${SOUND_PREFIX}
         SOURCES "i2s_clock.cpp")

find_library(SNDFILE_LIB sndfile)
if(SNDFILE_LIB)
    vp_model_compile_options(NAME i2s_speaker OPTIONS "-DUSE_SNDFILE")
    vp_model_link_libraries(NAME i2s_speaker LIBRARY sndfile)

    vp_model_compile_options(NAME i2s_microphone OPTIONS "-DUSE_SNDFILE")
    vp_model_link_libraries(NAME i2s_microphone LIBRARY sndfile)
endif()
