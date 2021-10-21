add_library(i2c_helper OBJECT "i2c_helper.cpp")
target_include_directories(i2c_helper PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(i2c_helper PRIVATE gvsoc)
