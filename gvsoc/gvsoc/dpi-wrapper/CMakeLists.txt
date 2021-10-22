cmake_minimum_required(VERSION 3.10)
project(gvsoc-dpi)

set(GVSOC_DPI_SRCS "src/dpi.cpp")

set(GVSOC_DPI_INC_DIRS "ext/sv/include/questa/"
                       "ext/nosv/")

add_library(gvsocdpi SHARED ${GVSOC_DPI_SRCS})
target_include_directories(gvsocdpi PRIVATE ${GVSOC_DPI_INC_DIRS})
target_link_libraries(gvsocdpi PUBLIC gvsoc)

install(TARGETS gvsocdpi
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
        )
