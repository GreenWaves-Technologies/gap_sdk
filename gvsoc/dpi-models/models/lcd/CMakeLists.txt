dpi_model(NAME lcd_ili9341
    SOURCES "ili9341.cpp"
    )

# use sdl2 if possible
include(FindPkgConfig)
pkg_search_module(SDL2 sdl2)
if(SDL2_FOUND)
    target_compile_options(lcd_ili9341 PRIVATE "-D__USE_SDL__")
    target_include_directories(lcd_ili9341 PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(lcd_ili9341 PRIVATE ${SDL2_LIBRARIES})
else()
    message(FATAL_ERROR "SDL2 not found!")
endif()
