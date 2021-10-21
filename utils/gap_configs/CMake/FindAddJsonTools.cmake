# Look for jsontools - if not present then install it
# set JSON_TOOLS_FOUND if existing json tools found
# update PYTHON_PATH to allow json tools to be found
# if JSON_TOOLS_FOUND is not set json-tools dependency should be added to main CMakeLists target
# as appropriate

function(find_add_json_tools repo tag)
    if(NOT DEFINED PYTHON_PATH)
        SET(PYTHON_PATH $ENV{PYTHONPATH} PARENT_SCOPE)
    endif()

    include(ExternalProject)

    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

    include(FindPythonModule)
    find_python_module(json_tools)

    if (PY_JSON_TOOLS)
        message(STATUS "Using installed Json-tools")
        SET(JSON_TOOLS_FOUND YES PARENT_SCOPE)
    else()
        SET(PYTHON_PATH "${CMAKE_INSTALL_PREFIX}/python:${PYTHON_PATH}" PARENT_SCOPE)
        find_python_module(json_tools)

        if (PY_JSON_TOOLS)
            message(STATUS "Using installed Json-tools")
            SET(JSON_TOOLS_FOUND YES PARENT_SCOPE)
        else()
            message(STATUS "Adding Json-tools install step from ${repo} tag ${tag}")
            include(ExternalProject)

            SET(JSON_TOOLS_FOUND NO PARENT_SCOPE)
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