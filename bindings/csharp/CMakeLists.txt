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

set(CSHARP_WRAPPER_DLL ${PROJECT_NAME}-sharp-cxx-wrap)

FIND_PACKAGE(SWIG REQUIRED)
include(UseSWIG)
INCLUDE_DIRECTORIES(
	${CMAKE_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR})

SET(CMAKE_SWIG_FLAGS "")
SET_SOURCE_FILES_PROPERTIES(../${PROJECT_NAME}.i PROPERTIES CPLUSPLUS ON)

# outdir for .so file
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set_property(SOURCE ../${PROJECT_NAME}.i PROPERTY SWIG_MODULE_NAME "libm2k")

if(ENABLE_TOOLS)
	execute_process(
		COMMAND ${SWIG_EXECUTABLE} -csharp -c++ -DCOMMUNICATION -outdir ${CMAKE_CURRENT_BINARY_DIR} -o ${CMAKE_CURRENT_BINARY_DIR}/libm2kCSHARP_wrap.cxx -dllimport ${CSHARP_WRAPPER_DLL}
		-I${CMAKE_SOURCE_DIR}/include -I${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../${PROJECT_NAME}.i
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		OUTPUT_VARIABLE swig_generated_output
		RESULT_VARIABLE swig_generated_res
		ERROR_VARIABLE swig_generated_error
		INPUT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../${PROJECT_NAME}.i
		)
else()
	execute_process(
			COMMAND ${SWIG_EXECUTABLE} -csharp -c++ -outdir ${CMAKE_CURRENT_BINARY_DIR} -o ${CMAKE_CURRENT_BINARY_DIR}/libm2kCSHARP_wrap.cxx -dllimport ${CSHARP_WRAPPER_DLL}
			-I${CMAKE_SOURCE_DIR}/include -I${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../${PROJECT_NAME}.i
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			OUTPUT_VARIABLE swig_generated_output
			RESULT_VARIABLE swig_generated_res
			ERROR_VARIABLE swig_generated_error
			INPUT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../${PROJECT_NAME}.i
	)
endif()

if (${swig_generated_res})
	message("------- SWIG failed to generate the C# wrapper.")
		message(${swig_generated_error})
endif()

#############################################################################################
##	build a libm2k-sharp-wrapper.dll which contains the CSHARP_wrapper.cxx generated
##	by SWIG
#############################################################################################

set(SRC_WRAPPER_FILE ${CMAKE_CURRENT_BINARY_DIR}/libm2kCSHARP_wrap.cxx)
set_source_files_properties(${SRC_WRAPPER_FILE} PROPERTIES GENERATED TRUE)

if(APPLE)
	# build universal binaries by default
	set(CMAKE_OSX_ARCHITECTURES "x86_64")
endif()

include_directories(
	SYSTEM
	${CMAKE_CURRENT_BINARY_DIR}
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_SOURCE_DIR}/include
	${IIO_INCLUDE_DIRS})

add_library(${CSHARP_WRAPPER_DLL} SHARED ${SRC_WRAPPER_FILE})

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	target_compile_options(${CSHARP_WRAPPER_DLL}
			PRIVATE
			-Wno-maybe-uninitialized)
endif()

target_include_directories(${CSHARP_WRAPPER_DLL} PRIVATE ${CMAKE_SOURCE_DIR}/include)

#Link the required dependencies
target_link_libraries(${CSHARP_WRAPPER_DLL}
	${PROJECT_NAME}
	${IIO_LIBRARIES})

#Take care of the library properties
set_target_properties(${CSHARP_WRAPPER_DLL} PROPERTIES
	VERSION ${PROJECT_VERSION}
	SOVERSION ${PROJECT_VERSION}
	)


install(TARGETS ${CSHARP_WRAPPER_DLL}
	ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
	)


#################################################################################
## build the C# dll using MCS/CSC and add the Assembly Info
#################################################################################

if (WIN32)
	set(MCS_EXECUTABLE_NAME csc)
else()
	set(MCS_EXECUTABLE_NAME mcs)
endif()

find_program(MCS_EXECUTABLE
	NAMES ${MCS_EXECUTABLE_NAME}
	HINTS "C:/Windows/Microsoft.NET/Framework/v4.0.30319"
	PATHS ENV MCS_EXECUTABLE_PATH
	DOC "C# compiler")
	mark_as_advanced(MCS_EXECUTABLE)

	if (MCS_EXECUTABLE)
		set(LIBM2K_CS_PC_IN "${CMAKE_CURRENT_SOURCE_DIR}/libm2k-sharp.pc.cmakein")
		set(LIBM2K_CS_PC "${CMAKE_CURRENT_BINARY_DIR}/libm2k-sharp.pc")
		configure_file(${LIBM2K_CS_PC_IN} ${LIBM2K_CS_PC} @ONLY)
		if(NOT SKIP_INSTALL_ALL)
			install(FILES ${LIBM2K_CS_PC} DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig")
		endif()

		set(LIBM2K_CS_DLL_CONFIG_IN "${CMAKE_CURRENT_SOURCE_DIR}/libm2k-sharp.dll.config.cmakein")
		set(LIBM2K_CS_DLL_CONFIG "${CMAKE_CURRENT_BINARY_DIR}/libm2k-sharp.dll.config")
		configure_file(${LIBM2K_CS_DLL_CONFIG_IN} ${LIBM2K_CS_DLL_CONFIG} @ONLY)
		if(NOT SKIP_INSTALL_ALL)
			install(FILES ${LIBM2K_CS_DLL_CONFIG} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cli/libm2k-sharp-${VERSION})
		endif()

		set(LIBM2K_CS_VERSION ${VERSION}.0.0)
		set(LIBM2K_CS_INFO_IN ${CMAKE_CURRENT_SOURCE_DIR}/AssemblyInfo.cs.in)
		set(LIBM2K_CS_INFO ${CMAKE_CURRENT_BINARY_DIR}/AssemblyInfo.cs)
		configure_file(${LIBM2K_CS_INFO_IN} ${LIBM2K_CS_INFO} @ONLY)

		set(LIBM2K_CS_DLL "${CMAKE_BINARY_DIR}/libm2k-sharp.dll")



		FILE(GLOB CSHARP_SRC_LIST ${CMAKE_CURRENT_BINARY_DIR}/*.cs)
		set_source_files_properties(${CSHARP_SRC_LIST} PROPERTIES GENERATED TRUE)

		set(LIBM2K_CS_SOURCES ${CSHARP_SRC_LIST})

		add_definitions(-DNO_ERROR_CATCHING) #disable internal error catching and allow swig to do the error catching itself

		foreach(SRC ${LIBM2K_CS_SOURCES})
			file(TO_NATIVE_PATH ${SRC} TMP)
			set(LIBM2K_CS_SOURCES_REALPATH ${LIBM2K_CS_SOURCES_REALPATH} ${TMP})
		endforeach(SRC)

		file(TO_NATIVE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/key.snk SIGN_KEY)
		file(TO_NATIVE_PATH ${LIBM2K_CS_DLL} LIBM2K_CS_DLL_OUT)

		add_custom_command(OUTPUT ${LIBM2K_CS_DLL}
			COMMAND ${MCS_EXECUTABLE} /target:library /out:${LIBM2K_CS_DLL_OUT} /keyfile:${SIGN_KEY} ${LIBM2K_CS_SOURCES_REALPATH}
			DEPENDS ${LIBM2K_CS_SOURCES})

		add_custom_target(libm2k-sharp ALL DEPENDS ${LIBM2K_CS_DLL} ${CSHARP_WRAPPER_DLL})
		add_dependencies(libm2k-sharp ${CSHARP_WRAPPER_DLL})

	endif()
