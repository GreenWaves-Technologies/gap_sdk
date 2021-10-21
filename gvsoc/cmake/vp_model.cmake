set(VP_TARGET_TYPES "" CACHE INTERNAL "contains the types of target")

function(vp_set_target_types)
    cmake_parse_arguments(
        VP_TARGET_TYPES
        ""
        "BUILD_OPTIMIZED;BUILD_DEBUG;BUILD_RTL"
        ""
        ${ARGN}
        )
    if(${BUILD_OPTIMIZED} AND NOT "_optim" IN_LIST VP_TARGET_TYPES)
        message(STATUS "setting optimized")
        set(VP_TARGET_TYPES ${VP_TARGET_TYPES} "_optim" CACHE INTERNAL "")
    endif()
    if(${BUILD_DEBUG} AND NOT "_debug" IN_LIST VP_TARGET_TYPES)
        set(VP_TARGET_TYPES ${VP_TARGET_TYPES} "_debug" CACHE INTERNAL "")
    endif()
    if(${BUILD_RTL} AND NOT "_sv" IN_LIST VP_TARGET_TYPES)
        set(VP_TARGET_TYPES ${VP_TARGET_TYPES} "_sv" CACHE INTERNAL "")
    endif()
endfunction()

# vp_model function
function(vp_model)
    cmake_parse_arguments(
        VP_MODEL
        ""
        "NAME;PREFIX;OUTPUT_NAME"
        "SOURCES;INCLUDES"
        ${ARGN}
        )
    #message(STATUS "vp_model: name=\"${VP_MODEL_NAME}\", output_name=\"${VP_MODEL_OUTPUT_NAME}\" prefix=\"${VP_MODEL_PREFIX}\", srcs=\"${VP_MODEL_SOURCES}\", incs=\"${VP_MODEL_INCLUDES}\"")

    # TODO verify arguments
    set(VP_MODEL_NAME_OPTIM "${VP_MODEL_NAME}_optim")
    set(VP_MODEL_NAME_DEBUG "${VP_MODEL_NAME}_debug")
    set(VP_MODEL_NAME_SV "${VP_MODEL_NAME}_sv")

    # ==================
    # Optimized models
    # ==================
    if(${BUILD_OPTIMIZED})
        add_library(${VP_MODEL_NAME_OPTIM} MODULE ${VP_MODEL_SOURCES})
        target_link_libraries(${VP_MODEL_NAME_OPTIM} PRIVATE gvsoc gap_archi archi_pulp)
        set_target_properties(${VP_MODEL_NAME_OPTIM} PROPERTIES PREFIX "")
        target_compile_options(${VP_MODEL_NAME_OPTIM} PRIVATE "-D__GVSOC__")

        if(VP_MODEL_OUTPUT_NAME)
            set_target_properties(${VP_MODEL_NAME_OPTIM}
                PROPERTIES OUTPUT_NAME ${VP_MODEL_OUTPUT_NAME})
        else()
            set_target_properties(${VP_MODEL_NAME_OPTIM}
                PROPERTIES OUTPUT_NAME ${VP_MODEL_NAME})
        endif()


        install(TARGETS ${VP_MODEL_NAME_OPTIM}
            LIBRARY DESTINATION  "${GVSOC_MODELS_INSTALL_FOLDER}/${GVSOC_MODELS_OPTIM_INSTALL_FOLDER}/${VP_MODEL_PREFIX}"
            ARCHIVE DESTINATION  "${GVSOC_MODELS_INSTALL_FOLDER}/${GVSOC_MODELS_OPTIM_INSTALL_FOLDER}/${VP_MODEL_PREFIX}"
            RUNTIME DESTINATION  "${GVSOC_MODELS_INSTALL_FOLDER}/${GVSOC_MODELS_OPTIM_INSTALL_FOLDER}/${VP_MODEL_PREFIX}/bin"
            INCLUDES DESTINATION "${GVSOC_MODELS_INSTALL_FOLDER}/${GVSOC_MODELS_OPTIM_INSTALL_FOLDER}/${VP_MODEL_PREFIX}/include"
            )
    endif()

    # ==================
    # Debug models
    # ==================
    if(${BUILD_DEBUG})
        add_library(${VP_MODEL_NAME_DEBUG} MODULE ${VP_MODEL_SOURCES})
        target_link_libraries(${VP_MODEL_NAME_DEBUG} PRIVATE gvsoc_debug gap_archi archi_pulp)
        set_target_properties(${VP_MODEL_NAME_DEBUG} PROPERTIES PREFIX "")
        target_compile_options(${VP_MODEL_NAME_DEBUG} PRIVATE "-D__GVSOC__")
        target_compile_definitions(${VP_MODEL_NAME_DEBUG} PRIVATE "-DVP_TRACE_ACTIVE=1")

        if(VP_MODEL_OUTPUT_NAME)
            set(RENAME_DEBUG_NAME ${VP_MODEL_OUTPUT_NAME})
        else()
            set(RENAME_DEBUG_NAME ${VP_MODEL_NAME})
        endif()

        install(
            FILES $<TARGET_FILE:${VP_MODEL_NAME_DEBUG}>
            DESTINATION  "${GVSOC_MODELS_INSTALL_FOLDER}/${GVSOC_MODELS_DEBUG_INSTALL_FOLDER}/${VP_MODEL_PREFIX}"
            RENAME "${RENAME_DEBUG_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}"
            )
    endif()

    # ==================
    # RTL models
    # ==================

    if(${BUILD_RTL})
        add_library(${VP_MODEL_NAME_SV} MODULE ${VP_MODEL_SOURCES})
        target_link_libraries(${VP_MODEL_NAME_SV} PRIVATE gvsoc_debug gap_archi archi_pulp)
        set_target_properties(${VP_MODEL_NAME_SV} PROPERTIES PREFIX "")
        target_compile_options(${VP_MODEL_NAME_SV} PRIVATE "-D__GVSOC__")
        target_compile_definitions(${VP_MODEL_NAME_SV}
            PRIVATE
            "-DVP_TRACE_ACTIVE=1"
            "-D__VP_USE_SYSTEMV=1"
            )

        if(VP_MODEL_OUTPUT_NAME)
            set(RENAME_SV_NAME ${VP_MODEL_OUTPUT_NAME})
        else()
            set(RENAME_SV_NAME ${VP_MODEL_NAME})
        endif()

        install(
            FILES $<TARGET_FILE:${VP_MODEL_NAME_SV}>
            DESTINATION  "${GVSOC_MODELS_INSTALL_FOLDER}/${GVSOC_MODELS_SV_INSTALL_FOLDER}/${VP_MODEL_PREFIX}"
            RENAME "${RENAME_SV_NAME}${CMAKE_SHARED_LIBRARY_SUFFIX}"
            )
    endif()
endfunction()

function(vp_model_link_libraries)
    cmake_parse_arguments(
        VP_MODEL
        ""
        "NAME;"
        "LIBRARY"
        ${ARGN}
        )

    foreach (TARGET_TYPE IN LISTS VP_TARGET_TYPES)
        set(VP_MODEL_NAME_TARGET "${VP_MODEL_NAME}${TARGET_TYPE}")
        target_link_libraries(${VP_MODEL_NAME_TARGET} PRIVATE ${VP_MODEL_LIBRARY})
    endforeach()
endfunction()

function(vp_model_compile_options)
    cmake_parse_arguments(
        VP_MODEL
        ""
        "NAME;"
        "OPTIONS"
        ${ARGN}
        )
    foreach (TARGET_TYPE IN LISTS VP_TARGET_TYPES)
        set(VP_MODEL_NAME_TYPE "${VP_MODEL_NAME}${TARGET_TYPE}")
        target_compile_options(${VP_MODEL_NAME_TYPE} PRIVATE ${VP_MODEL_OPTIONS})
    endforeach()
endfunction()

function(vp_model_compile_definitions)
    cmake_parse_arguments(
        VP_MODEL
        ""
        "NAME;"
        "DEFINITIONS"
        ${ARGN}
        )
    foreach (TARGET_TYPE IN LISTS VP_TARGET_TYPES)
        set(VP_MODEL_NAME_TYPE "${VP_MODEL_NAME}${TARGET_TYPE}")
        target_compile_definitions(${VP_MODEL_NAME_TYPE} PRIVATE ${VP_MODEL_DEFINITIONS})
    endforeach()
endfunction()

function(vp_model_include_directories)
    cmake_parse_arguments(
        VP_MODEL
        ""
        "NAME;"
        "DIRECTORY"
        ${ARGN}
        )
    foreach (TARGET_TYPE IN LISTS VP_TARGET_TYPES)
        set(VP_MODEL_NAME_TYPE "${VP_MODEL_NAME}${TARGET_TYPE}")
        target_include_directories(${VP_MODEL_NAME_TYPE} PRIVATE ${VP_MODEL_DIRECTORY})
    endforeach()
endfunction()

function(vp_model_sources)
    cmake_parse_arguments(
        VP_MODEL
        ""
        "NAME;"
        "SOURCES"
        ${ARGN}
        )
    foreach (TARGET_TYPE IN LISTS VP_TARGET_TYPES)
        set(VP_MODEL_NAME_TYPE "${VP_MODEL_NAME}${TARGET_TYPE}")
        target_sources(${VP_MODEL_NAME_TYPE} PRIVATE ${VP_MODEL_SOURCES})
    endforeach()
endfunction()

function(vp_files)
    cmake_parse_arguments(
        VP_FILES
        ""
        "PREFIX"
        "FILES"
        ${ARGN}
        )
    #message(STATUS "vp_files: prefix=\"${VP_FILES_PREFIX}\", files=\"${VP_FILES_FILES}\"")
    install(FILES ${VP_FILES_FILES}
        DESTINATION "python/${VP_FILES_PREFIX}")
endfunction()
