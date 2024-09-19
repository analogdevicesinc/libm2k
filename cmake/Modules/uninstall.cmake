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

set(MANIFEST "${CMAKE_BINARY_DIR}/install_manifest.txt")
if(NOT EXISTS ${MANIFEST})
    message(FATAL_ERROR "Cannot find install manifest: '${MANIFEST}'")
endif()
file(STRINGS ${MANIFEST} files)

set(MANIFEST "${CMAKE_BINARY_DIR}/install_manifest_py.txt")
if(EXISTS ${MANIFEST})
    file(STRINGS ${MANIFEST} installed_files)
else()
    message("Cannot find install manifest for Python bindings: '${MANIFEST}'")
endif()
set(INSTALLED_FILES ${files} ${installed_files})
set(PYTHON_EGG_DIR, "")

foreach(file ${INSTALLED_FILES})
    if(${file} MATCHES ".*python.*")
        set(file "/${file}")
    endif()
    if(EXISTS ${file} OR IS_SYMLINK ${file})
        if(${file} MATCHES ".*rules")
            continue()
        endif()
        if (${file} MATCHES ".*PKG-INFO")
          get_filename_component(PARENT_DIR ${file} PATH)
          get_filename_component(PARENT_DIR_NAME ${PARENT_DIR} NAME)
          if (PARENT_DIR_NAME MATCHES ".*libm2k.*" AND IS_DIRECTORY ${PARENT_DIR})
            set(PYTHON_EGG_DIR ${PARENT_DIR})
          endif ()
        endif ()

        message(STATUS "Removing file: '${file}'")

        exec_program(
            ${CMAKE_COMMAND} ARGS "-E remove ${file}"
            OUTPUT_VARIABLE stdout
            RETURN_VALUE result
        )

        if(NOT "${result}" STREQUAL 0)
            message(FATAL_ERROR "Failed to remove file: '${file}'.")
        endif()
    else()
        MESSAGE(STATUS "File '${file}' does not exist.")
    endif()
endforeach(file)

if(EXISTS ${PYTHON_EGG_DIR})
        message(STATUS "Removing Python egg directory: '${PYTHON_EGG_DIR}'")

        exec_program(
            ${CMAKE_COMMAND} ARGS "-E remove_directory ${PYTHON_EGG_DIR}"
            OUTPUT_VARIABLE stdout
            RETURN_VALUE result
        )

        if(NOT "${result}" STREQUAL 0)
            message(FATAL_ERROR "Failed to remove Python egg directory: '${PYTHON_EGG_DIR}'.")
        endif()
endif()

