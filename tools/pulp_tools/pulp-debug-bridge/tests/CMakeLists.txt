set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/tests)
set(CTEST_BINARY_DIRECTORY ${PROJECT_BINARY_DIR}/tests)


set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

file(GLOB files "*_test*.c" "*_test*.cpp")

message(STATUS "Adding tests")

set(WithValgrind ON)

set( CTEST_MEMORYCHECK_COMMAND valgrind )
set( CTEST_MEMORYCHECK_COMMAND_OPTIONS --error-exitcode=3 --leak-check=full)

function(add_memcheck_test name binary)
  set(memcheck_command "${CMAKE_MEMORYCHECK_COMMAND} ${CMAKE_MEMORYCHECK_COMMAND_OPTIONS}")
  separate_arguments(memcheck_command)
  add_test(${name} ${binary} ${ARGN})
  add_test(memcheck_${name} ${memcheck_command} ./${binary} ${ARGN})
endfunction(add_memcheck_test)

function(set_memcheck_test_properties name)
  set_tests_properties(${name} ${ARGN})
  set_tests_properties(memcheck_${name} ${ARGN})
endfunction(set_memcheck_test_properties)

set(EXTRA_CPP ../src/gdb-server/rsp-packet-codec.cpp)

foreach(file ${files})
	string(REGEX REPLACE "(^.*/|\\.[^.]*$)" "" file_without_ext ${file})
	add_executable(${file_without_ext} ${file} ${EXTRA_CPP})
    target_link_libraries(${file_without_ext} PRIVATE libplpbridge Threads::Threads)
	target_compile_options(${file_without_ext} PRIVATE -Wall -Werror)
	target_include_directories(${file_without_ext} PRIVATE ../include ../../json-tools/include)
	if(${WithValgrind})
		add_test(${file_without_ext} ${CTEST_MEMORYCHECK_COMMAND} ${CTEST_MEMORYCHECK_COMMAND_OPTIONS} ./${file_without_ext})
	else()
		add_test(${file_without_ext} ${file_without_ext})
	endif()
	set_tests_properties(${file_without_ext}
		PROPERTIES
		PASS_REGULAR_EXPRESSION "Test Passed")
	set_tests_properties(${file_without_ext}
		PROPERTIES
		FAIL_REGULAR_EXPRESSION "(Exception|Test Failed|ERROR SUMMARY: [^0]+[0-9]* errors from [^0]+[0-9]* contexts)")
	set_tests_properties(${file_without_ext}
		PROPERTIES
		TIMEOUT 120)
endforeach()