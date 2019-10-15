# Look for pulp_configs - if not present then install it

function(find_add_pulp_configs repo tag)
    if(EXISTS "${CMAKE_INSTALL_PREFIX}/configs" AND IS_DIRECTORY "${CMAKE_INSTALL_PREFIX}/configs")
        set(PULP_CONFIGS_FOUND YES PARENT_SCOPE)
    else()
        include(ExternalProject)
        message(STATUS "Add PULP configs from ${repo} tag ${tag}")
        ExternalProject_Add(
            pulp-configs
            GIT_REPOSITORY ${repo}
            GIT_TAG ${tag}
            CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DCMAKE_IGNORE_PATH=${CMAKE_IGNORE_PATH} -DJsonToolsGitRepo=${JsonToolsGitRepo} -DJsonToolsGitTag=${JsonToolsGitTag}
            LOG_DOWNLOAD ON
        )
    endif()
endfunction()