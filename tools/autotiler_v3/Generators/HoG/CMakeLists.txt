cmake_minimum_required(VERSION 3.5)

install_generator(HoG . ${GENERATORS_DIR} NO)

# HoG has an extra parameters file
install(FILES HoGParameters.h DESTINATION ${GENERATORS_DIR}/HoG/${KERNELS_DIR}/include)
# Also included by the test
target_include_directories(HoG_generator_test PRIVATE
    ${CMAKE_INSTALL_PREFIX}/include/${GENERATORS_DIR}/HoG/${KERNELS_DIR}/include
)