#
# Copyright (c) 2024 Analog Devices Inc.
#
# This file is part of libm2k
# (see http://www.github.com/analogdevicesinc/libm2k).
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 2.1 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#

cmake_minimum_required(VERSION 2.8.7)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(CMAKE_VERBOSE_MAKEFILE ON)
string(REGEX MATCHALL "([^\ ]+\ |[^\ ]+$)" CMAKE_CXX_LIST "${CMAKE_CXX_FLAGS}")
FOREACH(CXX_COMPILE_FLAG ${CMAKE_CXX_LIST})
	string(REPLACE " " "" CXX_COMPILE_FLAG "${CXX_COMPILE_FLAG}")
	set(EXTRA_COMPILE_FLAGS '${CXX_COMPILE_FLAG}',${EXTRA_COMPILE_FLAGS})
ENDFOREACH()

if(NOT ${CMAKE_VERSION} VERSION_LESS "3.13.5")
	# Check CMake Policy added in v3.13 regarding the SWIG generated target name
	# https://cmake.org/cmake/help/v3.14/policy/CMP0078.html
	cmake_policy(SET CMP0078 OLD)
endif()
set(SWIG_OUTPUT_NAME libm2k)

if(NOT ${CMAKE_VERSION} VERSION_LESS "3.14.7")
	# Check CMake Policy added in v3.14 regarding the SWIG_MODULE_NAME flag
	# https://cmake.org/cmake/help/v3.14/policy/CMP0086.html
	cmake_policy(SET CMP0086 NEW)
endif()

get_filename_component(IIO_LIBRARY_DIR ${IIO_LIBRARIES} DIRECTORY)

if(${CMAKE_VERSION} VERSION_LESS "3.12.0")
	if (NOT WIN32)
		set(Python_ADDITIONAL_VERSIONS 3)
	endif()

	include(FindPythonInterp)
	include(FindPythonLibs)

	set(Python_Interpreter_FOUND ${PYTHONINTERP_FOUND})
	set(Python_EXECUTABLE ${PYTHON_EXECUTABLE})
	set(Python_LIBRARIES ${PYTHON_LIBRARIES})
	set(Python_INCLUDE_PATH ${PYTHON_INCLUDE_PATH})
	set(Python_INCLUDE_DIRS ${PYTHON_INCLUDE_DIRS})
else()
	if(PYTHON_EXECUTABLE)
		set(Python_EXECUTABLE ${PYTHON_EXECUTABLE})
	endif()
	if (PYTHON_LIBRARY)
		set(Python_LIBRARY ${PYTHON_LIBRARY})
	endif()
	if (PYTHON_INCLUDE_DIR)
		set(Python_INCLUDE_DIR ${PYTHON_INCLUDE_DIR})
	endif()
	message(STATUS "bin=${Python_EXECUTABLE} lib=${Python_LIBRARY} inc=${Python_INCLUDE_DIR}")
	find_package(Python COMPONENTS Interpreter Development)
	message(STATUS "Python_EXECUTABLE ${Python_EXECUTABLE}")
endif()

FIND_PACKAGE(SWIG REQUIRED)
include(UseSWIG)

INCLUDE_DIRECTORIES(${Python_INCLUDE_DIRS}
	${CMAKE_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR})

if(ENABLE_TOOLS)
	set(CMAKE_SWIG_FLAGS -DCOMMUNICATION)
	set(EXTRA_COMPILE_FLAGS '-DCOMMUNICATION',${EXTRA_COMPILE_FLAGS})
else()
	set(CMAKE_SWIG_FLAGS "")
endif()

if(ENABLE_DOC)
	string(COMPARE LESS ${SWIG_VERSION} "4.0.0" SWIG_VERSION_ERROR)
	if(SWIG_VERSION_ERROR)
		message("-- SWIG Doxygen for Python disabled. SWIG 4.0.0 required (found version " ${SWIG_VERSION} " )")
	else()
		message("-- SWIG Doxygen for Python enabled (found version " ${SWIG_VERSION} " )")
		set(CMAKE_SWIG_FLAGS ${CMAKE_SWIG_FLAGS} "-doxygen")
		set(CMAKE_SWIG_FLAGS ${CMAKE_SWIG_FLAGS} -DDOXYGEN)
	endif()
endif()

SET_SOURCE_FILES_PROPERTIES(../${PROJECT_NAME}.i PROPERTIES CPLUSPLUS ON)

set(CMAKE_SWIG_OUTDIR ${CMAKE_BINARY_DIR})
set(SWIG_OUTFILE_DIR ${CMAKE_BINARY_DIR})
# outdir for .so file
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set_property(SOURCE ../${PROJECT_NAME}.i PROPERTY SWIG_MODULE_NAME "libm2k")

if(${CMAKE_VERSION} VERSION_LESS "3.8.2")
	swig_add_module(${SWIG_OUTPUT_NAME} python ../${PROJECT_NAME}.i)
else()
	SWIG_ADD_LIBRARY(${SWIG_OUTPUT_NAME} TYPE MODULE LANGUAGE python OUTPUT_DIR ${CMAKE_BINARY_DIR} OUTFILE_DIR ${CMAKE_BINARY_DIR} SOURCES ../${PROJECT_NAME}.i)
endif()

if (SWIG_MODULE_libm2k_REAL_NAME)
	set(SWIG_OUTPUT_NAME ${SWIG_MODULE_libm2k_REAL_NAME})
endif()

# Handle Anaconda issues on OSX
execute_process(COMMAND ${Python_EXECUTABLE} -c "import sysconfig; print(sysconfig.get_config_var('LDSHARED'))"
	RESULT_VARIABLE PY_CONF_VAR
	OUTPUT_VARIABLE PY_LD_SHARED
	OUTPUT_STRIP_TRAILING_WHITESPACE)

IF("${PY_LD_SHARED}" MATCHES "dynamic_lookup")
	set(PYTHON_LINK OFF)
ELSE()
	SET(PYTHON_LINK ON)
ENDIF()

IF (NOT PYTHON_LINK)
	IF (APPLE)
		message("--- Using LD_SHARED " ${PY_LD_SHARED})
		SET_TARGET_PROPERTIES(${SWIG_OUTPUT_NAME} PROPERTIES LINK_FLAGS "-flat_namespace -undefined dynamic_lookup")
		set(EXTRA_LINK_FLAGS '-flat_namespace','-undefined','dynamic_lookup',${EXTRA_LINK_FLAGS})
	ENDIF()
ENDIF()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	target_compile_options(_${PROJECT_NAME}
			PRIVATE
			-Wno-deprecated-declarations)
	set(EXTRA_COMPILE_FLAGS '-Wno-deprecated-declarations',${EXTRA_COMPILE_FLAGS})
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	target_compile_options(_${PROJECT_NAME}
			PRIVATE
			-Wno-unused-but-set-variable)
	set(EXTRA_COMPILE_FLAGS '-Wno-unused-but-set-variable',${EXTRA_COMPILE_FLAGS})
endif()

target_include_directories(_${PROJECT_NAME} PRIVATE
	${Python_INCLUDE_DIRS}
	${CMAKE_SOURCE_DIR}/include)

target_link_libraries(_${PROJECT_NAME} ${PROJECT_NAME})

IF (PYTHON_LINK)
	target_link_libraries(_${PROJECT_NAME} ${Python_LIBRARIES})
ENDIF()

string(REPLACE " " "','" CXX_COMPILE_FLAG "${CMAKE_SHARED_LIBRARY_CXX_FLAGS}")
set(EXTRA_COMPILE_FLAGS '${CXX_COMPILE_FLAG}',${EXTRA_COMPILE_FLAGS})

if(NOT APPLE)
	string(REPLACE " " "','" CXX_LINK_FLAG "${CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS}")
	set(EXTRA_LINK_FLAGS '${CXX_LINK_FLAG}',${EXTRA_LINK_FLAGS})
endif()

set(PYTHON_EXECUTABLE_LIB_VERSION ${Python_EXECUTABLE})
if(UNIX)
	if(NOT APPLE)
		set_target_properties(${SWIG_OUTPUT_NAME} PROPERTIES
			INSTALL_RPATH	"$ORIGIN")
	else()
		set_target_properties(${SWIG_OUTPUT_NAME} PROPERTIES
			SUFFIX ".so"
			INSTALL_RPATH	"@loader_path")
#		set_property(TARGET ${SWIG_MODULE_pyBar_REAL_NAME} APPEND PROPERTY
#			LINK_FLAGS "-flat_namespace -undefined suppress")
		set(EXTRA_LINK_FLAGS '-flat_namespace','-undefined','suppress',${EXTRA_LINK_FLAGS})
		set(EXTRA_LINK_FLAGS '-F${IIO_LIBRARY_DIR}','-F${CMAKE_BINARY_DIR}/src',${EXTRA_LINK_FLAGS})
		set(EXTRA_LINK_FLAGS '-framework','${PROJECT_NAME}',${EXTRA_LINK_FLAGS})
		set(EXTRA_LINK_FLAGS '-framework','iio',${EXTRA_LINK_FLAGS})
		if (NOT CMAKE_OSX_DEPLOYMENT_TARGET)
			set(CMAKE_OSX_DEPLOYMENT_TARGET ${MACOSX_DEPLOYMENT_TARGET})
		endif()
		set(EXTRA_LINK_FLAGS '-mmacosx-version-min=${CMAKE_OSX_DEPLOYMENT_TARGET}',${EXTRA_LINK_FLAGS})
		string(REPLACE " " "','" CXX_COMPILE_FLAG "${CMAKE_SHARED_MODULE_CREATE_CXX_FLAGS}")
		set(EXTRA_LINK_FLAGS '${CXX_COMPILE_FLAG}',${EXTRA_LINK_FLAGS})
	endif()
endif()

if (WIN32)
  # disable lib prefix on windows with mingw
  set_target_properties(${SWIG_OUTPUT_NAME} PROPERTIES PREFIX "")
endif()

set(SETUP_PY_IN ${CMAKE_CURRENT_SOURCE_DIR}/setup.py.cmakein)
set(SETUP_PY ${CMAKE_BINARY_DIR}/setup.py)
configure_file(${SETUP_PY_IN} ${SETUP_PY})

configure_file(${CMAKE_SOURCE_DIR}/bindings/libm2k.i ${CMAKE_BINARY_DIR}/libm2k.i COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/bindings/python/README.md ${CMAKE_BINARY_DIR}/README.md COPYONLY)

add_custom_target(${PROJECT_NAME}py ALL DEPENDS _${PROJECT_NAME} WORKING_DIRECTORY
	${CMAKE_BINARY_DIR} COMMAND ${PYTHON_EXECUTABLE_LIB_VERSION} ${SETUP_PY} build)

if(NOT SKIP_INSTALL_ALL)
	install(CODE "execute_process(WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		COMMAND ${PYTHON_EXECUTABLE_LIB_VERSION} ${SETUP_PY} install
		--record=${CMAKE_BINARY_DIR}/install_manifest_py.txt
		--root=\$ENV{DESTDIR}/ --prefix=${CMAKE_INSTALL_PREFIX})")
endif()
