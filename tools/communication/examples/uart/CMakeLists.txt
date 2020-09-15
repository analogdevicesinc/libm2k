cmake_minimum_required(VERSION 3.1.3)

set(CMAKE_CXX_STANDARD 11)

project(uart LANGUAGES CXX VERSION ${LIBM2K_VERSION})

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${IIO_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/include
)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} libm2k)

if (NOT WIN32)
        find_library(PTHREAD_LIBRARIES pthread)
        if (PTHREAD_LIBRARIES)
                target_link_libraries(${PROJECT_NAME} ${PTHREAD_LIBRARIES})
        endif()
endif()
