cmake_minimum_required(VERSION 3.1.3)
project(m2kcli)

set(CMAKE_CXX_STANDARD 14)

if (MSVC)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../deps/wingetopt/src)
    set(GETOPT_C_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../deps/wingetopt/src/getopt.c)
endif (MSVC)


include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${IIO_INCLUDE_DIR}
        ${CMAKE_SOURCE_DIR}/include
)

FILE(GLOB SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/*/*/*.cpp)

add_executable(${PROJECT_NAME} ${SRC_LIST} ${GETOPT_C_FILE})

target_link_libraries(${PROJECT_NAME} libm2k)

if (NOT SKIP_INSTALL_ALL)
    if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION /Library/Frameworks/m2k.framework/Tools)
    else ()
        install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
    endif ()
endif ()