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

cmake_minimum_required(VERSION 3.1.3)
project(doc)

find_program(DOXYGEN_PATH doxygen)
if (NOT DOXYGEN_PATH)
    message(FATAL_ERROR "Doxygen not found!")
endif()


set(SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include/libm2k)
set(DOXY_WORKING_DIR ${CMAKE_CURRENT_BINARY_DIR}/doxygen_doc)


set(PROJECT_NAME "libm2k")
set(DOCUMENTED_FILES "${SOURCES_DIR}/m2k.hpp \\
                      ${SOURCES_DIR}/contextbuilder.hpp \\
                      ${SOURCES_DIR}/context.hpp \\
                      ${SOURCES_DIR}/digital/m2kdigital.hpp \\
                      ${SOURCES_DIR}/digital/enums.hpp \\
                      ${SOURCES_DIR}/analog/m2kanalogin.hpp \\
                      ${SOURCES_DIR}/analog/m2kanalogout.hpp \\
                      ${SOURCES_DIR}/analog/m2kpowersupply.hpp \\
                      ${SOURCES_DIR}/analog/dmm.hpp \\
                      ${SOURCES_DIR}/analog/enums.hpp \\
                      ${SOURCES_DIR}/digital/enums.hpp \\
                      ${SOURCES_DIR}/enums.hpp \\
		       ${SOURCES_DIR}/m2khardwaretrigger.hpp \\
		       ${SOURCES_DIR}/enums.hpp \\
                     ${CMAKE_CURRENT_SOURCE_DIR}/mainpage.dox
                      "
)


configure_file(
    Doxyfile_API.in
    ${DOXY_WORKING_DIR}/Doxyfile.doxy
)


if(ENABLE_PYTHON)
	find_package(Sphinx REQUIRED)

	if (NOT SPHINX_EXECUTABLE)
	    message(FATAL_ERROR "Sphinx not found!")
	endif()

	set(BINARY_SPHINX_DIR "${DOXY_WORKING_DIR}/python/html/sphinx")
	# configured documentation tools and intermediate build results
	set(BINARY_BUILD_DIR "${BINARY_SPHINX_DIR}/build")
	# Sphinx cache with pickled ReST documents
	set(SPHINX_CACHE_DIR "${BINARY_SPHINX_DIR}/build/doctrees")
	# HTML output directory
	set(SPHINX_HTML_DIR "${BINARY_SPHINX_DIR}/build/html")

	configure_file(
		"${CMAKE_CURRENT_SOURCE_DIR}/sphinx/source/conf.py.in"
		"${BINARY_SPHINX_DIR}/build/conf.py"
		@ONLY)
	add_custom_target(libm2k_sphinx_doc
		${SPHINX_EXECUTABLE}
		-q -b html
		-c "${BINARY_BUILD_DIR}"
		-d "${SPHINX_CACHE_DIR}"
		"${CMAKE_CURRENT_SOURCE_DIR}/sphinx/source"
		"${SPHINX_HTML_DIR}"
		COMMENT "---Building HTML documentation with Sphinx for Python Bindings")

endif()

add_custom_target(doc
    COMMAND ${DOXYGEN_PATH} Doxyfile.doxy
    COMMAND ${CMAKE_COMMAND} -E make_directory ${DOXY_WORKING_DIR}/html
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/img ${DOXY_WORKING_DIR}/html/img
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/sphinx ${DOXY_WORKING_DIR}/html/sphinx
    WORKING_DIRECTORY ${DOXY_WORKING_DIR}
)

if(ENABLE_PYTHON)
	add_dependencies(doc libm2k_sphinx_doc)
endif()
