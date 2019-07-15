cmake_minimum_required(VERSION 3.1.3)
project(doc)

find_program(DOXYGEN_PATH doxygen)
if (NOT DOXYGEN_PATH)
    message(FATAL_ERROR "Doxygen not found!")
endif()


set(SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include/libm2k)
set(DOXY_WORKING_DIR ${CMAKE_CURRENT_BINARY_DIR}/doxygen_doc)


set(PROJECT_NAME "LibM2K")
set(DOCUMENTED_FILES "${SOURCES_DIR}/m2k.hpp \\
                      ${SOURCES_DIR}/digital/m2kdigital.hpp \\
                      ${SOURCES_DIR}/digital/enums.hpp \\
                      ${SOURCES_DIR}/analog/m2kanalogin.hpp \\
                      ${SOURCES_DIR}/analog/m2kanalogout.hpp \\
                      ${SOURCES_DIR}/analog/m2kpowersupply.hpp \\
                      ${SOURCES_DIR}/analog/dmm.hpp \\
                      ${SOURCES_DIR}/analog/enums.hpp \\
                      "
)


configure_file(
    Doxyfile_API.in
    ${DOXY_WORKING_DIR}/Doxyfile.doxy
)


add_custom_target(doc
    COMMAND ${DOXYGEN_PATH} Doxyfile.doxy
    COMMAND ${CMAKE_COMMAND} -E make_directory ${DOXY_WORKING_DIR}/html
    WORKING_DIRECTORY ${DOXY_WORKING_DIR}
)
