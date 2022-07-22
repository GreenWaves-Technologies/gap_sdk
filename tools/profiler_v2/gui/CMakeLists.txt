cmake_minimum_required(VERSION 3.14)

project(profiler VERSION 1.0.0 LANGUAGES CXX)

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#sets the compilation mode to Debug.
set(CMAKE_BUILD_TYPE Debug)

set_target_properties(${profiler} PROPERTIES AUTOMOC TRUE)

# Widgets finds its own dependencies.
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Charts REQUIRED)
#find_package(profiler_backend REQUIRED)

set(HEADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/inc)
#set(GUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/gui)

include_directories(${HEADER_DIR})

subdirs(src)
