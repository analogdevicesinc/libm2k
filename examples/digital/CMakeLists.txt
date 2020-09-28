cmake_minimum_required(VERSION 3.1.3)
set (CMAKE_CXX_STANDARD 11)
project(digital LANGUAGES CXX)


if (LIBM2K_VERSION)			# in-source build
	set(PROJECT_VERSION ${LIBM2K_VERSION})
else()					# standalone build
	set(PROJECT_VERSION "1.0.0")
	find_package(libm2k REQUIRED)
endif()

if (NOT WIN32)
	find_library(PTHREAD_LIBRARIES pthread)
endif()

add_executable(${PROJECT_NAME} "main.cpp")
add_executable(${PROJECT_NAME}_stream_test "stream_test.cpp")
target_link_libraries(${PROJECT_NAME} PRIVATE libm2k::libm2k)
target_link_libraries(${PROJECT_NAME}_stream_test PRIVATE libm2k::libm2k ${PTHREAD_LIBRARIES})
