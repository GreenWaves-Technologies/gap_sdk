# Look for jsontools - if not present then install it

function(find_add_json_tools repo tag)
    if(NOT DEFINED PYTHON_PATH)
        SET(PYTHON_PATH $ENV{PYTHONPATH})
    endif()

    include(ExternalProject)

    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

    include(FindPythonModule)
    find_python_module(json_tools)

    if (PY_JSON_TOOLS)
        message(STATUS "Using installed Json-tools")
    else()
        SET(PYTHON_PATH "${CMAKE_INSTALL_PREFIX}/python:${PYTHON_PATH}")
        find_python_module(json_tools)

        if (PY_JSON_TOOLS)
            message(STATUS "Using installed Json-tools")
        else()
            message(STATUS "Adding Json-tools install step")
            message(STATUS "Cloning from ${repo} TAG ${tag}")
            include(ExternalProject)

            set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external)

            # Add in JSON-TOOLS
            ExternalProject_Add(
                json-tools
                GIT_REPOSITORY ${repo}
                GIT_TAG ${tag}
                CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DCMAKE_IGNORE_PATH=${CMAKE_IGNORE_PATH}
                LOG_DOWNLOAD ON
            )
        endif()
    endif()
endfunction()