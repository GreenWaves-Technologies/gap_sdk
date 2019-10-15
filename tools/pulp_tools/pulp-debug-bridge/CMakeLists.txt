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
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

include(ExternalProject)
include(FindAddJsonTools)
include(FindAddPulpConfigs)

set(FTDI_DEVKIT_NAME "libftdi1-1.4git_devkit_x86_x64_14June2018.zip")
set(FTDI_DEVKIT_URL "https://sourceforge.net/projects/picusb/files/${FTDI_DEVKIT_NAME}/download")

option(WithFrameBuffer "Build with framebuffer support" ON)
option(WithFTDI "Build with FTDI support" ON)
option(WithTests "Build tests" OFF)

set(LibFTDIVersion v1.4 CACHE STRING "LibFTDI version/git tag to build")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug"
        CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
        FORCE
    )
endif(NOT CMAKE_BUILD_TYPE)

set(DEST_BIN bin)
set(DEST_CONFIGS configs)
set(DEST_MODULES python)
set(DEST_LIBS lib)
set(DEST_INCLUDE include)
set(DEST_BRIDGE ${DEST_MODULES}/bridge)

# For linux and MAC use fixed rpath for non system libraries
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${DEST_LIBS}")
set(CMAKE_SKIP_BUILD_RPATH  FALSE)
set(CMAKE_MACOSX_RPATH TRUE)

# when building, don't use the install RPATH already
# (but later on when installing)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
if("${isSystemDir}" STREQUAL "-1")
   set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
endif("${isSystemDir}" STREQUAL "-1")

# check platform
if (WIN32)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(WIN64 YES)
  endif()
elseif (APPLE)
  set(INSTALL_CMD ginstall)
else()
  set(INSTALL_CMD install)
endif()

enable_testing()

add_subdirectory(src)
if(WithTests)
  add_subdirectory(tests)
endif()


add_custom_target(plpbridge ALL)

# +++++ plpbridge library
add_library(libplpbridge MODULE 
  src/cables/adv_dbg_itf/adv_dbg_itf.cpp
  src/cables/jtag-proxy/jtag-proxy.cpp
  src/cables/jtag.cpp
  src/python_wrapper.cpp
  src/gdb-server/gdb-server.cpp
  src/gdb-server/rsp.cpp
  src/gdb-server/rsp-packet-codec.cpp
  src/gdb-server/target.cpp
  src/gdb-server/breakpoints.cpp
  src/reqloop.cpp
  src/bridge-commands.cpp
  src/reqserver.cpp
)

# Enable all warnings as errors
if(MSVC)
  target_compile_options(libplpbridge PRIVATE /W4 /WX)
else(MSVC)
  if (APPLE)
    target_compile_options(libplpbridge PUBLIC -Wall -Wextra -Werror -Wno-unused-private-field -Wno-nested-anon-types)
  else(APPLE)
    target_compile_options(libplpbridge PRIVATE -Wall -Wextra -Werror)
  endif(APPLE)
endif(MSVC)

# All include directories for libplpbridge
target_include_directories(libplpbridge PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include/cables
  ${CMAKE_CURRENT_SOURCE_DIR}/include/debug_bridge
  ${CMAKE_INSTALL_PREFIX}/include
)

# link with json-tools
target_link_libraries(libplpbridge PRIVATE ${CMAKE_INSTALL_PREFIX}/lib/libjson.a events)

# link windows sockets
if (WIN32)
  target_link_libraries(libplpbridge PRIVATE wsock32 ws2_32)
endif()

# Optionally add in FTDI support
if (WithFTDI)
  if (WIN32)
    if (WIN64)
      message(STATUS "Compiling in FTDI support")
      # Download LibFTDI SDK if on windows
      ExternalProject_Add(
          ftdi
          PREFIX ${CMAKE_BINARY_DIR}/ftdi
          URL ${FTDI_DEVKIT_URL}
          DOWNLOAD_NAME ${FTDI_DEVKIT_NAME}
          CONFIGURE_COMMAND ""
          BUILD_COMMAND ""
          INSTALL_COMMAND ""
          LOG_DOWNLOAD ON
      )
      ExternalProject_Get_Property(ftdi source_dir)
      target_include_directories(libplpbridge
          PRIVATE
              ${CMAKE_CURRENT_SOURCE_DIR}/include/cables/ftdi
              ${source_dir}/include/libftdi
      )

      set(FTDI_BIN_DIR ${source_dir}/bin64)
      target_link_libraries(libplpbridge PRIVATE ${FTDI_BIN_DIR}/libftdi1.dll)
    else()
      if (ForceFTDI)
        message(FATAL_ERROR "No Win32 bit support for FTDI")
      else()
        message(STATUS "FTDI is not available for Win32. Not including.")
      endif()
    endif()
  else()
    message(STATUS "Compiling in FTDI support")
    ExternalProject_Add(
        ftdi
        PREFIX ${CMAKE_BINARY_DIR}/ftdi
        GIT_REPOSITORY "git://developer.intra2net.com/libftdi"
        GIT_TAG ${LibFTDIVersion}
        CMAKE_ARGS -DLIB_SUFFIX= -DSTATICLIBS=OFF -DBUILD_TESTS=OFF -DDOCUMENTATION=OFF -DEXAMPLES=OFF -DFTDIPP=OFF -DFTDI_EEPROM=OFF -DPYTHON_BINDINGS=OFF -DCMAKE_INSTALL_PREFIX=./install -DCMAKE_INSTALL_RPATH=${CMAKE_INSTALL_RPATH} -DCMAKE_MACOSX_RPATH=${CMAKE_MACOSX_RPATH}
        LOG_DOWNLOAD ON
    )

    ExternalProject_Get_Property(ftdi binary_dir)
    target_include_directories(libplpbridge
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/include/cables/ftdi
            ${binary_dir}/install/include/libftdi1
    )
    set(FTDI_LIB_DIR ${binary_dir}/install/lib)
    if(APPLE)
      target_link_libraries(libplpbridge PRIVATE ${FTDI_LIB_DIR}/libftdi1.dylib)
    else()
      target_link_libraries(libplpbridge PRIVATE ${FTDI_LIB_DIR}/libftdi1.so)
    endif()

  endif()
  target_sources(libplpbridge PRIVATE src/cables/ftdi/ftdi.cpp)
  target_compile_definitions(libplpbridge PUBLIC __USE_FTDI__)
  add_dependencies(libplpbridge ftdi)
endif()

# Optionally add in SDL/Framebuffer support
if (WithFrameBuffer)
  if (NOT WIN32)
    find_package(SDL2 REQUIRED)
    if (SDL2_LIBRARY)
      message(STATUS "Compiling with framebuffer support")
      target_compile_definitions(libplpbridge PUBLIC __USE_SDL__)
      target_include_directories(libplpbridge PRIVATE ${SDL2_INCLUDE_DIR})
      target_link_libraries(libplpbridge PRIVATE ${SDL2_LIBRARY})
    else()
      message(WARNING "Can't find SDL2 library - Framebuffer support will not be included")
    endif()
  else()
    message(FATAL_ERROR "No Windows support for framebuffer")
  endif()
endif()

set_target_properties(libplpbridge PROPERTIES
  OUTPUT_NAME "pulpdebugbridge" # lib will be added as appropriate
)

add_dependencies(plpbridge libplpbridge)

# PULP bridge library

install(
  TARGETS libplpbridge
  DESTINATION ${DEST_BRIDGE}
)

# python tool binaries

install(
  DIRECTORY bin/ DESTINATION ${DEST_BIN}
  FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

# includes needed by other modules

if(DEFINED TARGET_INSTALL_DIR AND TARGET_INSTALL_DIR)
  get_filename_component(ABS_TARGET_INSTALL_DIR ${TARGET_INSTALL_DIR} ABSOLUTE BASE_DIR ${CMAKE_CURRENT_LIST_DIR})
  message(STATUS "TARGET_INSTALL_DIR is defined: ${ABS_TARGET_INSTALL_DIR}")
  install(
    DIRECTORY include/debug_bridge DESTINATION ${ABS_TARGET_INSTALL_DIR}/include
    FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
  )
else()
  message(STATUS "TARGET_INSTALL_DIR not defined")
endif()

install(
  DIRECTORY include/debug_bridge DESTINATION ${DEST_INCLUDE}
  FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

# ftdi libs on Linux/MAC

if(DEFINED FTDI_LIB_DIR)
  install(
    DIRECTORY ${FTDI_LIB_DIR}/ DESTINATION ${DEST_LIBS}
    FILES_MATCHING PATTERN "libftdi1*"
    PATTERN cmake EXCLUDE
    PATTERN pkgconfig EXCLUDE
  )
endif()

# Python module files

install(
  DIRECTORY python/ DESTINATION ${DEST_MODULES}
  FILE_PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ WORLD_EXECUTE
  FILES_MATCHING PATTERN "*.py"
)

# FTDI deps in bin dir if on windows

if(WithFTDI AND WIN32)
  install(
    DIRECTORY ${FTDI_BIN_DIR}/ DESTINATION ${DEST_BIN}
    FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
    FILES_MATCHING PATTERN "*.dll"
  )
endif()
