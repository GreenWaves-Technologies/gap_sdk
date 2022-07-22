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

#
# This file intends to initialize Kconfig tool in cmake build system.
# Here are the supported features :
# - Searching for Application's Kconfig if it exists
# - Generate default configurations
# - Execute kconfig parser script
# - Create target's menuconfig rule
# - Create cmake/kconfig dependencies
#

###############################################################################
# File definition
###############################################################################

if("$ENV{KCONFIG_CONFIG}" STREQUAL "")
    message(FATAL_ERROR "KCONFIG_CONFIG environnement variable has not been exported. Please add export KCONFIG_CONFIG=sdk.config to your .bashrc")
endif()

# Setting kconfig sdk output filename
set(SDKCONFIG_NAME                  $ENV{KCONFIG_CONFIG})
# Kconfig output file location
set(SDKCONFIG_FILE                  "${CMAKE_BINARY_DIR}/${SDKCONFIG_NAME}")
# Setting kconfig output parsed file for cmake uses
set(CMAKECONFIG_NAME                "cmake.config")
#CMake config file location
set(CMAKECONFIG_FILE               "${CMAKE_BINARY_DIR}/${CMAKECONFIG_NAME}")
# Kconfig to cmake parser
set(GEN_SDKCONFIG_CMAKE_TOOL        "${CMAKE_SETUP_DIR}/gencmakeconfig.py")
# Application's Kconfig file
set(KCONFIG_APP_DIR                 "${CMAKE_CURRENT_SOURCE_DIR}")
set(KCONFIG_APP                     "${KCONFIG_APP_DIR}/Kconfig")
# SDK's main Kconfig file
set(KCONFIG_SDK_DIR                 "${GAP_SDK_HOME}/utils/kconfig")
set(KCONFIG_SDK                     "${KCONFIG_SDK_DIR}/Kconfig")

###############################################################################
# Syntax coloring
###############################################################################

set(KCONFIG_MSG_PREFIX              "[${BoldGreen}Kconfig${ColourReset}]")

###############################################################################
# Macros
###############################################################################

# Create makefile's menuconfig rule
macro(setup_menuconfig)

    message(STATUS "${KCONFIG_MSG_PREFIX} Searching for KConfig...")
    if(EXISTS "${KCONFIG_APP}")
        message(STATUS "${KCONFIG_MSG_PREFIX} Application's Kconfig found")
        set(KCONFIG_ROOT ${KCONFIG_APP})
    else()
        message(STATUS "${KCONFIG_MSG_PREFIX} Application's Kconfig not found, searching for other KConfig")
        # TODO search for other kconfig here
        if(EXISTS "${KCONFIG_SDK}")
            message(STATUS "${KCONFIG_MSG_PREFIX} SDK's Kconfig found")
            set(KCONFIG_ROOT ${KCONFIG_SDK})
        else()
            message(FATAL_ERROR "Unable to find Kconfig file. Can't build menuconfig rule")
        endif()
    endif()

    #If KConfig has been found
    if(DEFINED KCONFIG_ROOT)
        message(STATUS "${KCONFIG_MSG_PREFIX} Root Kconfig location : ${KCONFIG_ROOT}")
        if(NOT EXISTS ${SDKCONFIG_FILE})
            # Create default configuration here with allddefconfig
            message(STATUS "${KCONFIG_MSG_PREFIX} Creating default configuration '$ENV{KCONFIG_CONFIG}' here: ${CMAKE_CURRENT_BINARY_DIR}")
            execute_process(COMMAND alldefconfig ${KCONFIG_ROOT}
                            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
        endif()

        #Create an empty cmake.config file if it not exists
        if(NOT EXISTS ${CMAKECONFIG_FILE})
            message(STATUS "${KCONFIG_MSG_PREFIX} Creating default cmake configuration")
            file(WRITE ${CMAKECONFIG_FILE})
            message(STATUS "${KCONFIG_MSG_PREFIX} Parsing default configuration")
            execute_process(COMMAND ${Python_EXECUTABLE} ${GEN_SDKCONFIG_CMAKE_TOOL} --config ${SDKCONFIG_FILE} --cmakeconfig ${CMAKECONFIG_FILE})
        endif()

        include(${CMAKECONFIG_FILE})

        # Add menuconfig target
        add_custom_target(  menuconfig
                            COMMENT "Invoking: menuconfig"
                            COMMAND ${Python_EXECUTABLE} -m menuconfig ${KCONFIG_ROOT}
                            USES_TERMINAL
                            COMMAND ${Python_EXECUTABLE} ${GEN_SDKCONFIG_CMAKE_TOOL} --config ${SDKCONFIG_FILE} --cmakeconfig ${CMAKECONFIG_FILE})
    endif()

    # Set a dependency between cmake and kconfig generated files to rerun
    # cmake when the configuration changed to read new options value
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${SDK_CONFIG_FILE})
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${GEN_SDKCONFIG_CMAKE_TOOL})
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${CMAKECONFIG_FILE})

endmacro()
