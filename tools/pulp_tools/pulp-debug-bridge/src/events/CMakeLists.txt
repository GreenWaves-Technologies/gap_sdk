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

set (CMAKE_CXX_STANDARD 11)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(default_build_type "Debug")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)

  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

enable_testing()

find_package (Threads)

add_library(ae STATIC
    ae/ae.c
)

target_include_directories(ae PRIVATE
    ../../include/events/ae
    ../../include/events
    ../../include
)

if (WIN32)
  target_link_libraries(ae PUBLIC wsock32 ws2_32)
endif()

set_property(TARGET ae PROPERTY C_STANDARD 99)

target_link_libraries(ae ${CMAKE_THREAD_LIBS_INIT})

target_compile_options(ae PRIVATE -Wall -Wextra -Werror)

# target_compile_definitions(ae PRIVATE AE_LOOP_DEBUG)

add_library(events STATIC
    circular-buffer.cpp
    events.cpp
    tcp-events.cpp
)

target_include_directories(events PRIVATE
    ../../include/events/ae
    ../../include/events
    ../../include/log
    ../../include
)

target_link_libraries(events log ae)

target_compile_options(events PRIVATE -Wall -Wextra -Werror)

list(APPEND PROJECT_LIBS "${CMAKE_THREAD_LIBS_INIT}" "ae" "events")
list(APPEND PROJECT_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/include ${CMAKE_CURRENT_LIST_DIR}/include/ae)
set(PROJECT_LIBS ${PROJECT_LIBS} PARENT_SCOPE)
set(PROJECT_INCLUDE_DIRS ${PROJECT_INCLUDE_DIRS} PARENT_SCOPE)
