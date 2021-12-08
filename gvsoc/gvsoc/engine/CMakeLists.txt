cmake_minimum_required(VERSION 3.10)
project(gvsoc-engine LANGUAGES CXX C)

set(GVSOC_ENGINE_CXX_SRCS
    "src/trace/lxt2.cpp"
    "src/trace/event.cpp"
    "src/trace/trace.cpp"
    "src/trace/raw/trace_dumper.cpp"
    "src/trace/raw.cpp"
    "src/trace/fst.cpp"
    "src/trace/vcd.cpp"
    "src/clock/clock.cpp"
    "src/vp.cpp"
    "src/proxy.cpp"
    "src/power/power_table.cpp"
    "src/power/power_engine.cpp"
    "src/power/component_power.cpp"
    "src/power/power_trace.cpp"
    "src/power/power_source.cpp"
    "src/launcher.cpp"
    )

set(GVSOC_ENGINE_C_SRCS
    "src/trace/fst/lz4.c"
    "src/trace/fst/fastlz.c"
    "src/trace/fst/fstapi.c"
    "src/trace/lxt2_write.c"
    )

set(GVSOC_ENGINE_INC_DIRS "include")

#"vp/clock_domain_impl.cpp"
#"vp/time_domain_impl.cpp"
#"vp/power_engine_impl.cpp"
#"vp/trace_domain_impl.cpp"

# ==================
# Optimized launcher
# ==================

if(${BUILD_OPTIMIZED})
    add_library(gvsoc SHARED ${GVSOC_ENGINE_CXX_SRCS} ${GVSOC_ENGINE_C_SRCS})
    target_include_directories(gvsoc PUBLIC ${GVSOC_ENGINE_INC_DIRS})
    target_link_libraries(gvsoc PUBLIC json-tools z pthread ${CMAKE_DL_LIBS})
    set_target_properties(gvsoc PROPERTIES OUTPUT_NAME "pulpvp")
    install(TARGETS gvsoc
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
        )

    add_executable(gvsoc_launcher "src/main.cpp")
    target_link_libraries(gvsoc_launcher PRIVATE gvsoc json-tools z pthread ${CMAKE_DL_LIBS})
    install(TARGETS gvsoc_launcher
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
        )
endif()

# ==============
# Debug launcher
# ==============

if(${BUILD_DEBUG})
    add_library(gvsoc_debug SHARED ${GVSOC_ENGINE_CXX_SRCS} ${GVSOC_ENGINE_C_SRCS})
    target_include_directories(gvsoc_debug PUBLIC ${GVSOC_ENGINE_INC_DIRS})
    target_link_libraries(gvsoc_debug PUBLIC json-tools z pthread ${CMAKE_DL_LIBS})
    set_target_properties(gvsoc_debug PROPERTIES OUTPUT_NAME "pulpvp-debug")
    install(TARGETS gvsoc_debug
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
        )
    target_compile_definitions(gvsoc_debug PRIVATE "-DVP_TRACE_ACTIVE=1")

    add_executable(gvsoc_launcher_debug "src/main.cpp")
    target_link_libraries(gvsoc_launcher_debug PRIVATE gvsoc_debug json-tools z pthread ${CMAKE_DL_LIBS})
    install(TARGETS gvsoc_launcher_debug
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
        )
    target_compile_definitions(gvsoc_launcher_debug PRIVATE "-DVP_TRACE_ACTIVE=1")
endif()

# ==============
# SV Library
# ==============

if(${BUILD_RTL})
    add_library(gvsoc_sv SHARED ${GVSOC_ENGINE_CXX_SRCS} ${GVSOC_ENGINE_C_SRCS})
    target_include_directories(gvsoc_sv PUBLIC ${GVSOC_ENGINE_INC_DIRS})
    target_link_libraries(gvsoc_sv PUBLIC json-tools z pthread ${CMAKE_DL_LIBS})
    set_target_properties(gvsoc_sv PROPERTIES OUTPUT_NAME "pulpvp-sv")
    install(TARGETS gvsoc_sv
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
        )
    target_compile_definitions(gvsoc_sv
        PRIVATE
        "-DVP_TRACE_ACTIVE=1"
        "-D__VP_USE_SYSTEMV=1"
        )
endif()

# ==============
# Subdirectories
# ==============

add_subdirectory(vp)
