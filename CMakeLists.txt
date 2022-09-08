#
# Copyright (c) 2019 Analog Devices Inc.
#
# This file is part of libm2k
# (see http://www.github.com/analogdevicesinc/libm2k).
#
# This program is free software: you can redistribute it and/or modify
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

cmake_minimum_required(VERSION 3.1.3)

if(NOT ${CMAKE_VERSION} VERSION_LESS "3.18.0")
	# https://cmake.org/cmake/help/v3.18/policy/CMP0068.html
	cmake_policy(SET CMP0068 NEW)
endif()

set(LIBM2K_VERSION_MAJOR 0)
set(LIBM2K_VERSION_MINOR 7)
set(LIBM2K_VERSION_PATCH 0)
set(LIBM2K_VERSION "${LIBM2K_VERSION_MAJOR}.${LIBM2K_VERSION_MINOR}.${LIBM2K_VERSION_PATCH}")

project(libm2k LANGUAGES CXX VERSION ${LIBM2K_VERSION})

include(GNUInstallDirs)
include(FindGit OPTIONAL)

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake/Modules)

# build a shared library by default
set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries")

# setup install location for MacOS frameworks
set(OSX_INSTALL_FRAMEWORKSDIR "/Library/Frameworks" CACHE STRING "Installation directory for frameworks")

# generate docs by default
option(ENABLE_DOC "Generate documentation with Doxygen" OFF)
option(BUILD_EXAMPLES "Build the default examples" OFF)
option(ENABLE_LOG "Build with logging support" OFF)
option(ENABLE_EXCEPTIONS "Build with exception handling support" ON)
option(ENABLE_PYTHON "Build Python bindings" ON)
option(ENABLE_CSHARP "Build C# bindings" OFF)
option(ENABLE_LABVIEW "Build LabVIEW bindings" OFF)
option(ENABLE_TOOLS "Build the tools" OFF)
option(INSTALL_UDEV_RULES "Install udev rules for the M2K" ON)

if (ENABLE_DOC)
    add_subdirectory(doc)
endif()

if (ENABLE_PYTHON OR ENABLE_CSHARP)
	FIND_PACKAGE(SWIG REQUIRED)
endif()

set(CMAKE_SHARED_LIBRARY_PREFIX "")
if (WIN32)
	set(CMAKE_SHARED_MODULE_PREFIX "")
	set(CMAKE_STATIC_LIBRARY_PREFIX "")
	SET(CMAKE_IMPORT_LIBRARY_PREFIX "")
endif()

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

set(LIBM2K_VERSION_FULL v${PROJECT_VERSION} - ${LIBM2K_VERSION_GIT})

if (ENABLE_LOG)
	find_package(glog)
	if (NOT TARGET glog::glog)
		find_library(GLOG_LIBRARIES NAMES glog)
		find_path(GLOG_INCLUDE_DIRS glog/logging.h)
		add_library(glog::glog SHARED IMPORTED)
		set_target_properties(glog::glog
				PROPERTIES
				INTERFACE_INCLUDE_DIRECTORIES ${GLOG_INCLUDE_DIRS}
				IMPORTED_LOCATION ${GLOG_LIBRARIES})
	endif()
	message(STATUS "Building with logging support(glog).")
endif()

#Enable or disable exception handling
if (NOT ENABLE_EXCEPTIONS)
	add_definitions(-D_EXCEPTIONS=0)
	if(NOT MSVC)
		remove_definitions("-fexceptions")
		add_definitions("-fno-exceptions")
	endif()
else()
	add_definitions(-D_EXCEPTIONS=1)
	if(NOT MSVC)
		remove_definitions("-fno-exceptions")
		add_definitions("-fexceptions")
	endif()
endif()

configure_file(${CMAKE_SOURCE_DIR}/version.hpp.cmakein ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}/version.hpp @ONLY)
configure_file(${CMAKE_SOURCE_DIR}/cmake/libm2kConfig.cmake ${CMAKE_BINARY_DIR}/cmake/${PROJECT_NAME}Config.cmake @ONLY)

# Set the default install path to /usr
if (NOT WIN32 AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        set(CMAKE_INSTALL_PREFIX "/usr/local" CACHE PATH "default install path" FORCE)
endif()

# get rid of Visual Studio's default "Debug" and "Release" output directories
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR})

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
endif()

set(INSTALL_PKGCONFIG_DIR "${CMAKE_INSTALL_LIBDIR}/pkgconfig"
	CACHE PATH "Installation directory for pkgconfig (.pc) files")
mark_as_advanced(INSTALL_PKGCONFIG_DIR)

# handle RPATH issues on OS X
if(APPLE)
	set(CMAKE_MACOSX_RPATH ON)
	set(CMAKE_SKIP_BUILD_RPATH TRUE)
	set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
	set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_LIBDIR}")
	set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
	list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_LIBDIR}" isSystemDir)
	if("${isSystemDir}" STREQUAL "-1")
		set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_LIBDIR}")
	endif()
endif()

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

#Find libiio and link it to the targets generated by this project
find_library(IIO_LIBRARIES NAMES iio libiio)
find_path(IIO_INCLUDE_DIRS iio.h)

#set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS}")
if (NOT WIN32)
	set(CMAKE_CXX_FLAGS "-Wall ${CMAKE_CXX_FLAGS}")
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -std=c++11")

#Add and build the source code subdirectory
add_subdirectory(src)

set(LIBM2K_PC ${CMAKE_CURRENT_BINARY_DIR}/libm2k.pc)
configure_file(libm2k.pc.cmakein ${LIBM2K_PC} @ONLY)
install(FILES ${LIBM2K_PC} DESTINATION "${INSTALL_PKGCONFIG_DIR}")

# install udev rules on Linux
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux" AND INSTALL_UDEV_RULES)
	set(LIBM2K_UDEV_RULES "${CMAKE_CURRENT_SOURCE_DIR}/53-adi-m2k-usb.rules")
	set(UDEV_RULES_PATH "/etc/udev/rules.d" CACHE STRING "Target directory for udev rule installation.")
	install(FILES ${LIBM2K_UDEV_RULES} DESTINATION ${UDEV_RULES_PATH})
endif()

configure_file(libm2k.iss.cmakein ${CMAKE_CURRENT_BINARY_DIR}/libm2k.iss @ONLY)

#Add and build libm2k tools
if (ENABLE_TOOLS)
	message("---- Building tools")
	add_subdirectory(tools)
	add_definitions(-DCOMMUNICATION)
endif()

#Add and build python bindings
if (ENABLE_PYTHON AND SWIG_FOUND)
	message("---- Building Python bindings")
	add_subdirectory(bindings/python)
endif()

#Add and build C# bindings
if (ENABLE_CSHARP AND SWIG_FOUND)
	message("---- Building C# bindings")
	add_subdirectory(bindings/csharp)
endif()

#Add and build LabVIEW wrapper bindings
if (ENABLE_LABVIEW)
	message("---- Building LabVIEW bindings")
	add_subdirectory(bindings/labview)
endif()

#Add and build the examples
if (BUILD_EXAMPLES)
	add_subdirectory(examples)
endif()

# Create an installer if compiling for OSX
if(OSX_PACKAGE)
	set(LIBM2K_PKG ${CMAKE_CURRENT_BINARY_DIR}/libm2k-${PROJECT_VERSION}.g${LIBM2K_VERSION_GIT}.pkg)
	set(LIBM2K_TEMP_PKG ${CMAKE_CURRENT_BINARY_DIR}/libm2k-${VERSION}-temp.pkg)
	set(LIBM2K_DISTRIBUTION_XML ${CMAKE_CURRENT_BINARY_DIR}/Distribution.xml)
	set(LIBM2K_FRAMEWORK_DIR ${CMAKE_CURRENT_BINARY_DIR}/src/libm2k.framework)
	set(LIBM2K_FRAMEWORK_HEADERS_DIR ${LIBM2K_FRAMEWORK_DIR}/Headers)
	configure_file(Distribution.xml.cmakein ${LIBM2K_DISTRIBUTION_XML} @ONLY)

	# Include the headers in the Framework directory, keeping the directory structure healthy.
	# Otherwise, the headers are not included in the .pkg and therefore not installed.
	file(COPY ${CMAKE_SOURCE_DIR}/include/libm2k/ DESTINATION "${LIBM2K_FRAMEWORK_HEADERS_DIR}/libm2k" FILES_MATCHING PATTERN "*.hpp")

	find_program(PKGBUILD_EXECUTABLE
		NAMES pkgbuild
		DOC "OSX Package builder (pkgbuild)")
	mark_as_advanced(PKGBUILD_EXECUTABLE)

	find_program(PRODUCTBUILD_EXECUTABLE
		NAMES productbuild
		DOC "OSX Package builder (productbuild)")
	mark_as_advanced(PRODUCTBUILD_EXECUTABLE)

	set(COPY_TOOLS_COMMAND)
	if (ENABLE_TOOLS)
		list(APPEND COPY_TOOLS_COMMAND
			COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:m2kcli> ${LIBM2K_FRAMEWORK_DIR}/Tools)
	endif()

	add_custom_command(OUTPUT ${LIBM2K_PKG}
		COMMAND ${CMAKE_COMMAND} -E make_directory ${LIBM2K_FRAMEWORK_DIR}/Tools
		${COPY_TOOLS_COMMAND}
		COMMAND ${PKGBUILD_EXECUTABLE}
			--component ${LIBM2K_FRAMEWORK_DIR}
			--identifier libm2k --version ${PROJECT_VERSION}
			--install-location ${OSX_INSTALL_FRAMEWORKSDIR} ${LIBM2K_TEMP_PKG}
		COMMAND ${PRODUCTBUILD_EXECUTABLE}
			--distribution ${LIBM2K_DISTRIBUTION_XML} ${LIBM2K_PKG}
		COMMAND ${CMAKE_COMMAND} -E remove ${LIBM2K_TEMP_PKG}
		DEPENDS ${PROJECT_NAME} ${M2KCLI_TARGET} ${LIBM2K_DISTRIBUTION_XML}
	)

	if (PKGBUILD_EXECUTABLE AND PRODUCTBUILD_EXECUTABLE)
		add_custom_target(libm2k-pkg ALL DEPENDS ${LIBM2K_PKG})

		install(CODE "execute_process(COMMAND /usr/sbin/installer -pkg ${LIBM2K_PKG} -target /)")
	else()
		message(WARNING "Missing pkgbuild or productbuild: OSX installer won't be created.")
	endif()
endif()

if (NOT OSX_PACKAGE)
	# Support creating some basic binpkgs via `make package`.
	# Disabled if OSX_PACKAGE is enabled, as tarballs would end up empty otherwise.
	option(ENABLE_PACKAGING "Create .deb/.rpm or .tar.gz packages via 'make package'" OFF)

	if(ENABLE_PACKAGING)
		if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
			include(cmake/DarwinPackaging.cmake)
		endif()
		if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
			include(cmake/LinuxPackaging.cmake)
		endif()
	endif()
endif()

add_custom_target(uninstall
    "${CMAKE_COMMAND}" -P "${CMAKE_MODULE_PATH}/uninstall.cmake"
)
