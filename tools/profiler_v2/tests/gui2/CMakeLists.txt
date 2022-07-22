cmake_minimum_required(VERSION 3.14)

project(helloworld VERSION 1.0.0 LANGUAGES CXX)

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

#sets the compilation mode to Debug.
set(CMAKE_BUILD_TYPE Debug)

set_target_properties(${main} PROPERTIES AUTOMOC TRUE)

# Widgets finds its own dependencies.
find_package(Qt5Widgets REQUIRED)
#find_package(profiler_backend REQUIRED)

add_executable(main main.cpp mainwindow.cpp timeline.cpp profiler_backend_user.cpp)

qt5_use_modules(main Widgets)
include_directories(main  "../../backend/include")

#target_link_libraries(main LINK_PUBLIC profiler_backend Qt5Widgets)
target_link_libraries(main profiler_backend)
#target_link_libraries(main PRIVATE )
