cmake_minimum_required(VERSION 3.10)
project(gvsoc-core)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(JSON_TOOLS_SRCS "../../utils/json-tools/src/jsmn.cpp"
                    "../../utils/json-tools/src/json.cpp")
set(JSON_TOOLS_INC_DIRS "../../utils/json-tools/include/")

add_library(json-tools OBJECT ${JSON_TOOLS_SRCS})
target_include_directories(json-tools PUBLIC ${JSON_TOOLS_INC_DIRS})

install(DIRECTORY bin/ DESTINATION bin USE_SOURCE_PERMISSIONS)

add_subdirectory(dpi-wrapper)
add_subdirectory(engine)
add_subdirectory(launcher)
add_subdirectory(models)
