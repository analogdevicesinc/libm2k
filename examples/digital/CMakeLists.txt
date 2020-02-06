cmake_minimum_required(VERSION 3.1.3)
set (CMAKE_CXX_STANDARD 11)

project(digital LANGUAGES CXX VERSION ${LIBM2K_VERSION})
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${IIO_INCLUDE_DIR}
	${CMAKE_SOURCE_DIR}/include
)

if (NOT WIN32)
	find_library(PTHREAD_LIBRARIES pthread)
endif()

add_executable(${PROJECT_NAME} "main.cpp")
add_executable(${PROJECT_NAME}_stream_test "stream_test.cpp")
target_link_libraries(${PROJECT_NAME} libm2k)
target_link_libraries(${PROJECT_NAME}_stream_test libm2k ${PTHREAD_LIBRARIES})
