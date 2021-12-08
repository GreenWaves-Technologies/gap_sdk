add_library(freertos_kernel INTERFACE)
target_include_directories(freertos_kernel INTERFACE "include")
target_include_directories(freertos_kernel INTERFACE "portable/GCC/RI5CY-GAP9")
