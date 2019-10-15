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

add_library(log STATIC
    log.cpp
)

target_include_directories(log PUBLIC
    ../../include/log
)

target_compile_options(log PRIVATE -Wall -Wextra)

list(APPEND PROJECT_LIBS log)
list(APPEND PROJECT_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/include)
set(LOG_LIB log PARENT_SCOPE)
set(PROJECT_LIBS ${PROJECT_LIBS} PARENT_SCOPE)
set(PROJECT_INCLUDE_DIRS ${PROJECT_INCLUDE_DIRS} PARENT_SCOPE)

