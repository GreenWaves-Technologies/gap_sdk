cmake_minimum_required(VERSION 3.5)

include_directories(${AT_HOME}/DSP_Libraries)
install_generator(Fir . ${GENERATORS_DIR} YES)
