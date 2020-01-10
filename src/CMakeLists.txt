cmake_minimum_required(VERSION 3.1.3)

option(OSX_PACKAGE "Create OSX package" OFF)
set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries")

if(APPLE)
	# build universal binaries by default
	set(CMAKE_OSX_ARCHITECTURES "x86_64")
endif()

SET(CMAKE_EXE_LINKER_FLAGS  "/FORCE")

include_directories(
	SYSTEM 
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/include
	${IIO_INCLUDE_DIRS}
	${LOG4CPP_INCLUDE_DIR}
)

FILE(GLOB_RECURSE SRC_LIST  *.cpp)

FILE(GLOB_RECURSE HEADERS_LIST ${CMAKE_SOURCE_DIR}/include/libm2k/*.hpp)

if (ENABLE_TOOLS)
	FILE(GLOB_RECURSE COMMUNICATION_SRC_LIST ../tools/communication/src/*.cpp)
	list(APPEND SRC_LIST ${COMMUNICATION_SRC_LIST})
else()
	foreach(filepath ${HEADERS_LIST})
		if(${filepath} MATCHES "${CMAKE_SOURCE_DIR}/include/libm2k/tools/*")
			list(REMOVE_ITEM HEADERS_LIST ${filepath})
		endif()
	endforeach()
endif()

#Create the libm2k library
add_library(${PROJECT_NAME} SHARED
			${SRC_LIST}
                        ${HEADERS_LIST}
			${DEVICES})

if (NOT WIN32)
	find_library(PTHREAD_LIBRARIES pthread)
	if (PTHREAD_LIBRARIES)
		target_link_libraries(libm2k ${PTHREAD_LIBRARIES})
	endif()
endif()


#Link the required dependencies
target_link_libraries(${PROJECT_NAME}
	${IIO_LIBRARIES}
        ${LOG4CPP_LIBRARies}
)

#Take care of the library properties
set_target_properties(${PROJECT_NAME} PROPERTIES
        VERSION ${PROJECT_VERSION}
	SOVERSION ${PROJECT_VERSION}
)

if (MSVC)
	set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME libm2k)
endif()


if (APPLE)
	set_target_properties(${PROJECT_NAME} PROPERTIES FRAMEWORK FALSE)
	set(CMAKE_MACOSX_RPATH ON)
	if (OSX_PACKAGE)
		set_target_properties(${PROJECT_NAME} PROPERTIES FRAMEWORK TRUE)
		set(OSX_INSTALL_FRAMEWORKSDIR "/Library/Frameworks" CACHE STRING "Installation directory for frameworks")
		get_filename_component(OSX_INSTALL_FRAMEWORKSDIR "${OSX_INSTALL_FRAMEWORKSDIR}" REALPATH BASE_DIR "${CMAKE_CURRENT_BINARY_DIR}")
		set(SKIP_INSTALL_ALL ${OSX_PACKAGE})
	endif()
endif()

install(TARGETS ${PROJECT_NAME}
	ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
	FRAMEWORK DESTINATION "${OSX_INSTALL_FRAMEWORKSDIR}"
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/libm2k/ DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/libm2k"
	FILES_MATCHING PATTERN "*.hpp")
