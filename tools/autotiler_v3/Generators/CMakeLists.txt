cmake_minimum_required(VERSION 3.5)

add_custom_target(generator_test)

add_subdirectory(MatAdd)
add_subdirectory(MatMult)
#add_subdirectory(HoG)
add_subdirectory(Fir)
add_subdirectory(FFT2DModel)
add_subdirectory(BilinearResizes)
add_subdirectory(IntegralImage)
