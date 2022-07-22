###############################################################################
# Copyright (c) 2022 GreenWaves Technologies SAS
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. Neither the name of GreenWaves Technologies SAS nor the names of its
#    contributors may be used to endorse or promote products derived from
#    this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
###############################################################################

# Designed to be included from a GAP app's CMakeLists.txt file

###############################################################################
# Variables
###############################################################################

# Configure env. variable "GAP_SDK_HOME" in cmakefile's scope

if($ENV{GAP_SDK_HOME} STREQUAL "")
    message(FATAL_ERROR "GAP_SDK_HOME environnement variable has not been exported.")
endif()

set(GAP_SDK_HOME $ENV{GAP_SDK_HOME})

# CMakefile's tools location
set(CMAKE_SETUP_DIR ${GAP_SDK_HOME}/utils/cmake)

if(NOT WIN32)
    string(ASCII 27 Esc)
    set(ColourReset "${Esc}[m")
    set(ColourBold  "${Esc}[1m")
    set(Red         "${Esc}[31m")
    set(Green       "${Esc}[32m")
    set(Yellow      "${Esc}[33m")
    set(Blue        "${Esc}[34m")
    set(Magenta     "${Esc}[35m")
    set(Cyan        "${Esc}[36m")
    set(White       "${Esc}[37m")
    set(BoldRed     "${Esc}[1;31m")
    set(BoldGreen   "${Esc}[1;32m")
    set(BoldYellow  "${Esc}[1;33m")
    set(BoldBlue    "${Esc}[1;34m")
    set(BoldMagenta "${Esc}[1;35m")
    set(BoldCyan    "${Esc}[1;36m")
    set(BoldWhite   "${Esc}[1;37m")
endif()

set(PYTHON_MSG_PREFIX "[${BoldCyan}Python${ColourReset}]")

# Python 
find_package(Python COMPONENTS Interpreter)
if(NOT DEFINED Python_FOUND)
    message(FATAL ERROR "${PYTHON_MSG_PREFIX} Interpreter not found")
else()
    message(STATUS "${PYTHON_MSG_PREFIX} Interpreter found here ${Python_EXECUTABLE}")
endif()


###############################################################################
# Includes
###############################################################################

include(${CMAKE_SETUP_DIR}/kconfig.cmake)   # Kconfig macros
include(${CMAKE_SETUP_DIR}/board.cmake)     # Board related macros
include(${CMAKE_SETUP_DIR}/macros.cmake)    # Generic macros

###############################################################################
# Macros
###############################################################################

# This macro must be executed at the beginning of the cmake build process. 
macro(setup_cmake)
    setup_menuconfig()
    setup_board()
    setupcrosscompile()
endmacro()