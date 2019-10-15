cmake_minimum_required(VERSION 3.5)
project(gap-flasher LANGUAGES C VERSION 1.0.0)

set(GAP_CONFIG ${CMAKE_CURRENT_LIST_DIR}/include/gap_config.h)
list(APPEND INCLUDE_DIRS
    ${CMAKE_CURRENT_LIST_DIR}/include
)
link_directories(${CMAKE_CURRENT_LIST_DIR}/lib/gap)

set(LIBRARIES rt rtio rt gcc)

list(APPEND SRCS src/flasher.c src/hyper_flash.c src/hyper_flash_commands.c)

add_executable(flasher src/flasher.c src/hyper_flash.c src/hyper_flash_commands.c)

target_compile_options(flasher PRIVATE -Wextra -Wall -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wundef -include ${GAP_CONFIG})
target_link_libraries(flasher rt rtio rt gcc)
target_include_directories(flasher PRIVATE ${INCLUDE_DIRS})

list(APPEND SRCS src/fuser.c)

add_executable(fuser src/fuser.c)

target_compile_options(fuser PRIVATE -Wextra -Wall -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wundef -include ${GAP_CONFIG})
target_link_libraries(fuser rt rtio rt gcc)
target_include_directories(fuser PRIVATE ${INCLUDE_DIRS})

install(
    TARGETS flasher
    DESTINATION bin
    PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)

install(
    TARGETS fuser
    DESTINATION bin
    PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ
)

install(
    FILES ${CMAKE_CURRENT_LIST_DIR}/bin/flashImageBuilder
    DESTINATION bin
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)
