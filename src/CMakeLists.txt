cmake_minimum_required(VERSION 3.1.3)
option(BUILD_STATIC_LIB "Build static library" OFF)

if(APPLE)
	# build universal binaries by default
	set(CMAKE_OSX_ARCHITECTURES "x86_64")
endif()

if(MSVC)
	set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS OFF)
endif()

include_directories(
	SYSTEM ${IIO_INCLUDE_DIRS}
        ${LOG4CPP_INCLUDE_DIR}
)


FILE(GLOB SRC_LIST  *.cpp *.cc */*.cpp */*/*.cpp)
FILE(GLOB HEADERS_LIST
	${CMAKE_SOURCE_DIR}/include/libm2k/*/*.hpp
        ${CMAKE_SOURCE_DIR}/include/libm2k/*.hpp
        ${CMAKE_SOURCE_DIR}/src/devices/*/*.hpp
        ${CMAKE_SOURCE_DIR}/src/*.hpp)

FILE(GLOB DEVICES ${CMAKE_SOURCE_DIR}/src/devices/*/*.cpp)


add_library(m2k SHARED
			${SRC_LIST}
                        ${HEADERS_LIST}
			${DEVICES})

if (WIN32 OR ENABLE_APPLICATION_BUNDLE)
	set_target_properties(m2k PROPERTIES OUTPUT_NAME m2k)
endif()	

if (NOT WIN32)
	find_library(PTHREAD_LIBRARIES pthread)
	if (PTHREAD_LIBRARIES)
		target_link_libraries(${PROJECT_NAME} ${PTHREAD_LIBRARIES})
	endif()
endif()


target_link_libraries(m2k
	${IIO_LIBRARIES}
        ${LOG4CPP_LIBRARies}
)

set_target_properties(m2k PROPERTIES
        VERSION ${PROJECT_VERSION}
	SOVERSION ${PROJECT_VERSION}
	PUBLIC_HEADER "${HEADERS_LIST}"
)

message(${CMAKE_INSTALL_LIBDIR})
install(TARGETS m2k
	ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
	PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/libm2k"
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/libm2k/ DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/libm2k"
	FILES_MATCHING PATTERN "*.hpp")
