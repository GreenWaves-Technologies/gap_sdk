cmake_minimum_required(VERSION 3.16.3)

project(sdk)

if(NOT DEFINED CONFIG_GAP_SDK_HOME)
    message(STATUS "Using current directory as GAP SDK Home")
    set(CONFIG_GAP_SDK_HOME ${CMAKE_CURRENT_SOURCE_DIR})
endif()

add_subdirectory(utils/littlefs)

set (VP_MODELS ${CONFIG_GAP_SDK_HOME}/gvsoc/gvsoc/models\;${CONFIG_GAP_SDK_HOME}/gvsoc/gvsoc_gap/models)

add_subdirectory(gvsoc)

if (EXISTS tools/sfu_gen/CMakeLists.txt)
    add_subdirectory(tools/sfu_gen)
endif()

## Audio fwk
set(AF_COMPONENTS_PATH ${CONFIG_GAP_SDK_HOME}/tools/audio-framework/components)
set(AF_LIBRARY_PATH ${CONFIG_GAP_SDK_HOME}/install/workstation/lib)
add_subdirectory(tools/audio-framework)

# add checkout target
add_custom_target(
    mini_checkout
    COMMENT "Checkouting minimal common sdk components"
    COMMAND git submodule update --init ${CONFIG_GAP_SDK_HOME}/rtos/pmsis
    COMMAND git submodule update --recursive --init ${CONFIG_GAP_SDK_HOME}/examples
    COMMAND git submodule update --init ${CONFIG_GAP_SDK_HOME}/utils/gapy
    COMMAND git submodule update --recursive --init ${CONFIG_GAP_SDK_HOME}/rtos/freeRTOS
    )

add_custom_target(
    checkout DEPENDS mini_checkout
    COMMAND git git submodule update --init --recursive ${CONFIG_GAP_SDK_HOME}
    )

add_custom_target(
    autotiler
    COMMENT "Building autotiler if required"
    COMMAND if [ -e ${CONFIG_GAP_SDK_HOME}/tools/autotiler_v3/Makefile ]\; then make -C ${CONFIG_GAP_SDK_HOME}/tools/autotiler_v3 all\; fi
    )


add_custom_target(
    legacy_makefile_sdk ALL
    COMMENT "Building modules using legacy build systems"
    DEPENDS autotiler
    )

add_custom_target(
    plptest
    COMMAND pip install --user -r ${CONFIG_GAP_SDK_HOME}/utils/plptest/requirements.txt
    COMMAND if [ -e ${CONFIG_GAP_SDK_HOME}/utils/plptest ]\; then cd ${CONFIG_GAP_SDK_HOME}/utils/plptest && make build\; fi
    )

add_custom_target(
    legacy_makefile_utils ALL
    DEPENDS plptest
    )

## Install pip requirements, note: does not install nntool reqs as building
## will do that
add_custom_target(
    pip_install
    COMMENT "install pip deps from requirements"
    COMMAND pip install --user -r ${CONFIG_GAP_SDK_HOME}/requirements.txt
    COMMAND pip install --user -r ${CONFIG_GAP_SDK_HOME}/nn_menu/requirements.txt
    COMMAND pip install --user -r ${CONFIG_GAP_SDK_HOME}/tools//nntool/requirements.txt
    COMMAND pip install --user -r ${CONFIG_GAP_SDK_HOME}/utils/gapy/requirements.txt
    )

