# Copyright 2016 Analog Devices, Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 3, or (at your option)
#  any later version.
# 
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
# 
#  You should have received a copy of the GNU General Public License
#  along with GNU Radio; see the file COPYING.  If not, write to
#  the Free Software Foundation, Inc., 51 Franklin Street,
#  Boston, MA 02110-1301, USA.

cmake_minimum_required(VERSION 3.1.3)
project(libm2k LANGUAGES CXX VERSION 1.00)
include(GNUInstallDirs)
include(FindGit OPTIONAL)

# build a shared library by default
set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries")
# generate docs by default
option(WITH_DOC "Generate documentation with Doxygen" ON)

# Get the GIT hash of the latest commit
if (GIT_FOUND)
	execute_process(
		COMMAND ${GIT_EXECUTABLE} rev-parse --show-toplevel
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		OUTPUT_VARIABLE LIBM2K_GIT_REPO
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)

	if (${LIBM2K_GIT_REPO} MATCHES ${CMAKE_CURRENT_SOURCE_DIR})
		execute_process(
			COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
			OUTPUT_VARIABLE LIBM2K_VERSION_GIT
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)
	endif()
endif()

if (NOT LIBM2K_VERSION_GIT)
	set(LIBM2K_VERSION_GIT v${PROJECT_VERSION})
endif()

# Set the default install path to /usr
if (NOT WIN32 AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set(CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "default install path" FORCE)
endif()

set(INSTALL_PKGCONFIG_DIR "${CMAKE_INSTALL_LIBDIR}/pkgconfig"
	CACHE PATH "Installation directory for pkgconfig (.pc) files")
mark_as_advanced(INSTALL_PKGCONFIG_DIR)

# handle RPATH issues on OS X
if(APPLE)
	set(CMAKE_MACOSX_RPATH ON)
	set(CMAKE_SKIP_BUILD_RPATH FALSE)
	set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
	set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_LIBDIR}")
	set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
	list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_LIBDIR}" isSystemDir)
	if("${isSystemDir}" STREQUAL "-1")
		set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_LIBDIR}")
	endif()
endif()

find_package(Doxygen)
if(DOXYGEN_FOUND)
	option(WITH_DOC "Generate documentation with Doxygen" ON)

	if (WITH_DOC)
		configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
		set(HTML_DEST_DIR ${CMAKE_CURRENT_BINARY_DIR}/html)
		file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/doc DESTINATION ${HTML_DEST_DIR})

		add_custom_command(TARGET m2k POST_BUILD
			COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
			COMMENT "Generating API documentation with Doxygen" VERBATIM
			)

		if(NOT SKIP_INSTALL_ALL)
			install(DIRECTORY ${HTML_DEST_DIR} DESTINATION ${CMAKE_INSTALL_DOCDIR})
		endif()
	endif()
else()
	message(STATUS "Doxygen not found, API documentation won't be generated")
endif()

add_subdirectory(src)
add_subdirectory(examples)

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
		"Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel."
		FORCE)
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS None Debug Release RelWithDebInfo MinSizeRel)
endif()

list(FIND CMAKE_CXX_COMPILE_FEATURES cxx_constexpr OUT_CONSTEXPR)
if (NOT "${OUT_CONSTEXPR}" STREQUAL "-1")
	add_definitions(-DHAS_CONSTEXPR=1)
endif()

set(VC_PATH "$ENV{VS120COMNTOOLS}/../../VC")
find_library(IIO_LIBRARIES NAMES iio libiio PATHS ${VC_PATH}/lib ${VC_PATH}/lib/amd64)

find_path(IIO_INCLUDE_DIRS iio.h PATHS ${VC_PATH}/include)

include_directories(
	${CMAKE_SOURCE_DIR}/include/
)

set(LIBM2K_PC ${CMAKE_CURRENT_BINARY_DIR}/libm2k.pc)
configure_file(libm2k.pc.cmakein ${LIBM2K_PC} @ONLY)
install(FILES ${LIBM2K_PC} DESTINATION "${INSTALL_PKGCONFIG_DIR}")
