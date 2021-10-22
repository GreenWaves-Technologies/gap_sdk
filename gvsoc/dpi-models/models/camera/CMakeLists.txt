dpi_model(NAME camera
    SOURCES "camera.cpp"
    )

# include graphicsmagick if exists
find_library(MAGICK_LIB GraphicsMagick++)
if(MAGICK_LIB)
    target_compile_options(camera PRIVATE "-D__MAGICK__")
    target_link_libraries(camera PRIVATE GraphicsMagick++)
else()
endif()
