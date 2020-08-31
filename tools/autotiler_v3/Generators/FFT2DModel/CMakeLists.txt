cmake_minimum_required(VERSION 3.5)

project(FFT_MODEL LANGUAGES C)

link_directories(
    ${CMAKE_INSTALL_PREFIX}/lib
)

add_executable(fft_twiddles_gen
    GenTwid_Swap.c
)

target_link_libraries(fft_twiddles_gen PRIVATE m)

add_custom_command(
    OUTPUT  SwapTablesDef.c TwiddlesDef.c SwapTablesDef.h TwiddlesDef.h
    COMMAND fft_twiddles_gen
    COMMENT "Generating FFT twiddles"
)

add_custom_target(fft_twiddles ALL
    DEPENDS SwapTablesDef.c TwiddlesDef.c SwapTablesDef.h TwiddlesDef.h
)

add_executable(FFT2D_generator_test
    FFT2DGenerator.c
    FFT2DGenTest.c
)

target_include_directories(FFT2D_generator_test PRIVATE
    ${CMAKE_INSTALL_PREFIX}/include
)

target_link_libraries(FFT2D_generator_test PRIVATE tile)

if(${WithFrameBuffer})
    target_link_libraries(FFT2D_generator_test PRIVATE SDL2)
    target_link_libraries(FFT2D_generator_test PRIVATE SDL2_ttf)
endif()

add_dependencies(FFT2D_generator_test libtile)

set(TEST_DIR ${CMAKE_CURRENT_BINARY_DIR}/AT_TEST)
file(MAKE_DIRECTORY ${TEST_DIR})

add_custom_command(
    OUTPUT ${TEST_DIR}/FFT2DKernels.c ${TEST_DIR}/FFT2DKernels.h
    COMMAND FFT2D_generator_test -o ${TEST_DIR}
    COMMENT "Generating FFT2D autotiler code"
)

get_target_property(TWIDDLES fft_twiddles GENERATED)

add_executable(FFT2D_runtime_test
    ${FFT_MODEL_SOURCE_DIR}/FFTBasicKernels.c
    ${TEST_DIR}/FFT2DKernels.c
    SwapTablesDef.c
    TwiddlesDef.c
    FFT2DRunTest.c
)

target_include_directories(FFT2D_runtime_test PRIVATE
    ${CMAKE_INSTALL_PREFIX}/include
    ${FFT_MODEL_SOURCE_DIR}
    ${TEST_DIR}
    ${TEST_DIR}/..
    ${GAP8_H_INCLUDE_DIR}
)

target_compile_definitions(FFT2D_runtime_test PRIVATE __EMUL__ FFTR2_128 FFTR4_256)
target_compile_options(FFT2D_runtime_test PRIVATE -m32)
target_link_libraries(FFT2D_runtime_test -m32)

add_dependencies(FFT2D_runtime_test fft_twiddles)

add_test(NAME FFT2D_runtime_test COMMAND $<TARGET_FILE:FFT2D_runtime_test>)

install(FILES FFT2DGenerator.c DESTINATION ${GENERATORS_DIR}/FFT2D)
install(FILES FFT2DGenTest.c DESTINATION ${GENERATORS_DIR}/FFT2D)
install(FILES FFT2DGenerator.h DESTINATION ${GENERATORS_DIR}/FFT2D)

install(FILES FFTBasicKernels.c DESTINATION ${GENERATORS_DIR}/FFT2D)
install(FILES FFTScalarBasicKernels.c DESTINATION ${GENERATORS_DIR}/FFT2D)
install(FILES FFTBasicKernels.h DESTINATION ${GENERATORS_DIR}/FFT2D)
install(FILES GenTwid_Swap.c DESTINATION ${GENERATORS_DIR}/FFT2D/twiddles_gen)
install(FILES HowToBuild.txt DESTINATION ${GENERATORS_DIR}/FFT2D/twiddles_gen)
