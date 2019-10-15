# Find if a Python module is installed
# Found at http://www.cmake.org/pipermail/cmake/2011-January/041666.html
# To use do: find_python_module(PyQt4 REQUIRED)

function(find_python_module module)
    include(FindPackageHandleStandardArgs)
    include(FindPythonInterp)

	if(NOT DEFINED PYTHON_PATH)
		SET(PYTHON_PATH $ENV{PYTHONPATH} PARENT_SCOPE)
	endif()

	message(STATUS "searching with PYTHONPATH ${PYTHON_PATH}")
	string(TOUPPER ${module} module_upper)

	if(NOT PY_${module_upper})
		if(ARGC GREATER 1 AND ARGV1 STREQUAL "REQUIRED")
			set(${module}_FIND_REQUIRED TRUE)
		endif()

		# A module's location is usually a directory, but for binary modules
        # it's a .so file.
        find_package(PythonInterp)
		execute_process(COMMAND cmake -E env PYTHONPATH=${PYTHON_PATH} "${PYTHON_EXECUTABLE}" "-c" 
			"import re, ${module}; print(re.compile('/__init__.py.*').sub('',${module}.__file__))"
			RESULT_VARIABLE _${module}_status 
			OUTPUT_VARIABLE _${module}_location
			ERROR_QUIET
			OUTPUT_STRIP_TRAILING_WHITESPACE)

		if(NOT _${module}_status)
			set(PY_${module_upper} ${_${module}_location} CACHE STRING 
				"Location of Python module ${module}")
		endif(NOT _${module}_status)
	endif(NOT PY_${module_upper})
	find_package_handle_standard_args(PY_${module} DEFAULT_MSG PY_${module_upper})
endfunction(find_python_module)