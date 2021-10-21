#
# Copyright (C) 2018 ETH Zurich and University of Bologna and GreenWaves Technologies SAS
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# Authors: Martin Croome, GreenWaves Technologies

cmake_minimum_required(VERSION 3.5)
project(json-tools VERSION 1.0.0)
set (CMAKE_CXX_STANDARD 11)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo"
        CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
        FORCE
    )
endif()

add_library(json STATIC
    src/jsmn.cpp
    src/json.cpp
)

target_include_directories(json PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(MSVC)
    target_compile_options(json PRIVATE /W4 /WX)
else(MSVC)
    target_compile_options(json PRIVATE -Wall -Wextra -pedantic -Werror)
endif(MSVC)

set_target_properties(json PROPERTIES
	VERSION ${PROJECT_VERSION}
	POSITION_INDEPENDENT_CODE ON
)

set(JSON_TOOLS_PYTHON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/python/)
set(JSON_TOOLS_PY_FILES 
    ${JSON_TOOLS_PYTHON_DIR}/json_tools.py
    ${JSON_TOOLS_PYTHON_DIR}/__init__.py
)

set(JSON_TOOLS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/)
set(JSON_TOOLS_INCLUDE_FILES 
    ${JSON_TOOLS_INCLUDE_DIR}/jsmn.h
    ${JSON_TOOLS_INCLUDE_DIR}/json.hpp
)

install(PROGRAMS ${JSON_TOOLS_PY_FILES} DESTINATION python)
install(FILES ${JSON_TOOLS_INCLUDE_FILES} DESTINATION include)
install(TARGETS json DESTINATION lib)
