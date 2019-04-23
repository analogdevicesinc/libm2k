cmake_minimum_required(VERSION 2.8.7)

FIND_PACKAGE(SWIG REQUIRED)
include(UseSWIG)

FIND_PACKAGE(PythonLibs)
find_package(PythonInterp REQUIRED)

INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH}
	${CMAKE_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR})

SET(CMAKE_SWIG_FLAGS "")

SET_SOURCE_FILES_PROPERTIES(${PROJECT_NAME}.i PROPERTIES CPLUSPLUS ON)

set(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR})
# outdir for .so file
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

SWIG_ADD_LIBRARY(${PROJECT_NAME} TYPE SHARED LANGUAGE python SOURCES ${PROJECT_NAME}.i)
#SWIG_LINK_LIBRARIES(libm2k ${PYTHON_LIBRARIES})

# ${SWIG_MODULE_pyBar_REAL_NAME} will resolve to _pyBar.so TARGET
target_include_directories(_${PROJECT_NAME} PRIVATE
	${PYTHON_INCLUDE_DIRS}
	${CMAKE_SOURCE_DIR}/include)

target_link_libraries(_${PROJECT_NAME}
	${PROJECT_NAME}
	${PYTHON_LIBRARIES})

if(UNIX)
	if(NOT APPLE)
		set_target_properties(${SWIG_MODULE_libm2k_REAL_NAME} PROPERTIES
			INSTALL_RPATH	"$ORIGIN")
	else()
		set_target_properties(${SWIG_MODULE_libm2k_REAL_NAME} PROPERTIES
			SUFFIX ".so"
			INSTALL_RPATH	"@loader_path")
#		set_property(TARGET ${SWIG_MODULE_pyBar_REAL_NAME} APPEND PROPERTY
#			LINK_FLAGS "-flat_namespace -undefined suppress")
	endif()
endif()

if (WIN32)
  # disable lib prefix on windows with mingw
  set_target_properties(${SWIG_MODULE_libm2k_REAL_NAME} PROPERTIES PREFIX "")
endif()

#set_property(TARGET ${SWIG_MODULE_pyBar_REAL_NAME} APPEND PROPERTY
#	LINK_FLAGS "-flat_namespace -undefined suppress")

set(SETUP_PY_IN ${CMAKE_CURRENT_SOURCE_DIR}/setup.py.cmakein)
set(SETUP_PY ${CMAKE_CURRENT_SOURCE_DIR}/setup.py)

configure_file(${SETUP_PY_IN} ${SETUP_PY})

add_custom_target(${PROJECT_NAME}py ALL DEPENDS ${SETUP_PY} COMMAND ${PYTHON_EXECUTABLE} ${SETUP_PY} --quiet build)

if(NOT SKIP_INSTALL_ALL)
	install(CODE "execute_process(WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMAND ${PYTHON_EXECUTABLE} ${SETUP_PY} install --root=\$ENV{DESTDIR}/ --prefix=${CMAKE_INSTALL_PREFIX})")
endif()
