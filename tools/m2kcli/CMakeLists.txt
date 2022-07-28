#/*
# * Copyright (c) 2019 Analog Devices Inc.
# *
# * This file is part of libm2k
# * (see http://www.github.com/analogdevicesinc/libm2k).
# *
# * This program is free software; you can redistribute it and/or
# * modify it under the terms of the GNU General Public License
# * as published by the Free Software Foundation; either version 2
# * of the License, or (at your option) any later version.
# *
# * This program is distributed in the hope that it will be useful,
# * but WITHOUT ANY WARRANTY; without even the implied warranty of
# * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# * GNU General Public License for more details.
# *
# * You should have received a copy of the GNU General Public License
# * along with this program; if not, write to the Free Software
# * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
# * */

cmake_minimum_required(VERSION 3.1.3)

set(CMAKE_CXX_STANDARD 11)

project(m2kcli LANGUAGES CXX C VERSION ${LIBM2K_VERSION})

if (MSVC)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../deps/wingetopt/src)
    set(GETOPT_C_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../deps/wingetopt/src/getopt.c)
endif (MSVC)

FILE(GLOB_RECURSE SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_SOURCE_DIR}/tools/communication/src/utils/*.cpp)

add_executable(${PROJECT_NAME} ${SRC_LIST} ${GETOPT_C_FILE})
target_include_directories(${PROJECT_NAME}
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
                ${CMAKE_CURRENT_SOURCE_DIR}/..
        )

target_link_libraries(${PROJECT_NAME} libm2k::libm2k)

if (NOT WIN32)
        find_library(PTHREAD_LIBRARIES pthread)
        if (PTHREAD_LIBRARIES)
                target_link_libraries(${PROJECT_NAME} ${PTHREAD_LIBRARIES})
        endif()
endif()

if (NOT SKIP_INSTALL_ALL)
	if((${CMAKE_SYSTEM_NAME} MATCHES "Darwin") AND OSX_PACKAGE)
	        install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${OSX_INSTALL_FRAMEWORKSDIR}/libm2k.framework/Tools)
        else()
		install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
        endif()
endif()

# Make the test targets available to the main CMakeLists.txt
set(M2KCLI_TARGET ${PROJECT_NAME} PARENT_SCOPE)
