cmake_minimum_required(VERSION 3.1.3)

set(CMAKE_CXX_STANDARD 11)

project(m2kcli LANGUAGES CXX C VERSION ${LIBM2K_VERSION})

if (MSVC)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../deps/wingetopt/src)
    set(GETOPT_C_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../deps/wingetopt/src/getopt.c)
endif (MSVC)


include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${IIO_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/include
)

FILE(GLOB_RECURSE SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp ${CMAKE_SOURCE_DIR}/tools/communication/src/utils/*.cpp)

add_executable(${PROJECT_NAME} ${SRC_LIST} ${GETOPT_C_FILE})

target_link_libraries(${PROJECT_NAME} libm2k)

if (NOT SKIP_INSTALL_ALL)
    install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif ()
