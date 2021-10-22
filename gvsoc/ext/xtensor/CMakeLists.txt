add_library(xtensor INTERFACE)
target_include_directories(xtensor INTERFACE "include")
target_compile_options(xtensor INTERFACE "-DXTENSOR_USE_XSIMD")
