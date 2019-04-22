cmake_minimum_required(VERSION 3.1.3)

if (MSVC)
	include_directories(${CMAKE_CURRENT_SOURCE_DIR})
endif(MSVC)

# get rid of Visual Studio's default "Debug" and "Release" output directories
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_BINARY_DIR})

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
endif()

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${IIO_INCLUDE_DIRS}
	${CMAKE_SOURCE_DIR}/include
)

add_executable(connect connect.cpp)
add_executable(siggen siggen.cpp)

#target_link_libraries(generic_connect libm2k ${LOG4CPP_LIBRARIES})
#target_link_libraries(m2k_connect m2k ${LOG4CPP_LIBRARIES})
#target_link_libraries(generic_dmm m2k ${LOG4CPP_LIBRARIES})
#target_link_libraries(m2k_ps m2k ${LOG4CPP_LIBRARIES})

target_link_libraries(connect
		${PROJECT_NAME}
		${LOG4CPP_LIBRARIES})
target_link_libraries(siggen
		${PROJECT_NAME}
		${LOG4CPP_LIBRARIES})
