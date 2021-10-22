cmake_minimum_required(VERSION 3.16.3)
project(gvsoc)

# gap_archi
add_library(gap_archi INTERFACE)
target_include_directories(gap_archi INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/../rtos/pulp/gap_archi/include")

# archi_pulp
add_library(archi_pulp INTERFACE)
target_include_directories(archi_pulp INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/../rtos/pulp/archi_pulp/include")

# install sub folder
set(GVSOC_MODELS_INSTALL_FOLDER       "python")
set(GVSOC_MODELS_OPTIM_INSTALL_FOLDER "")
set(GVSOC_MODELS_DEBUG_INSTALL_FOLDER "debug")
set(GVSOC_MODELS_SV_INSTALL_FOLDER    "sv")

# ================
# Utility includes
# ================
include(cmake/vp_model.cmake)

# =======
# Options
# =======
option(BUILD_OPTIMIZED "build GVSOC with optimizations"               ON)
option(BUILD_DEBUG     "build GVSOC with debug information"           ON)
option(BUILD_RTL       "build GVSOC for RTL simulation optimizations" ON)
option(SKIP_DPI "Do not build DPI" OFF)

set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-g -O3")
set(CMAKE_CC_FLAGS_RELWITHDEBINFO "-g -O3")

# verify supported chip
set(SUPPORTED_TARGET_CHIPS
    "GAP8"
    "GAP8_V2"
    "GAP8_V3"
    "GAP9"
    "GAP9_V2"
    )
if(NOT "${TARGET_CHIP}" IN_LIST SUPPORTED_TARGET_CHIPS)
    message(FATAL_ERROR "${TARGET_CHIP} chip is not supported by GVSOC")
endif()

# set target types
vp_set_target_types(
    BUILD_DEBUG ${BUILD_DEBUG}
    BUILD_OPTIMIZED ${BUILD_OPTIMIZED}
    BUILD_RTL ${BUILD_RTL}
    )

# ==================
# Add subdirectories
# ==================

add_subdirectory(ext)
add_subdirectory(dpi-models)
add_subdirectory(gvsoc)

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/gvsoc_gap_sfu")
    set(UDMA_HAS_SFU TRUE)
endif()

add_subdirectory(gvsoc_gap)

if(UDMA_HAS_SFU)
    add_subdirectory(gvsoc_gap_sfu)
endif()
