vp_model(NAME himax
    PREFIX "devices/camera"
    SOURCES "himax.cpp")

find_library(MAGICK_LIB GraphicsMagick++)
if(MAGICK_LIB)
    vp_model_compile_options(NAME himax OPTIONS "-D__MAGICK__")
    vp_model_link_libraries(NAME himax LIBRARY GraphicsMagick++)
else()
endif()
