cmake_minimum_required(VERSION 3.1.3)
set (CMAKE_CXX_STANDARD 11)
project(voltmeter LANGUAGES CXX)


if (LIBM2K_VERSION)			# in-source build
	set(PROJECT_VERSION ${LIBM2K_VERSION})
else()					# standalone build
	set(PROJECT_VERSION "1.0.0")
	find_package(libm2k REQUIRED)
endif()

add_executable(${PROJECT_NAME} "main.cpp")
target_link_libraries(${PROJECT_NAME} PRIVATE libm2k::libm2k)
