cmake_minimum_required(VERSION 2.8)
set (CMAKE_CXX_STANDARD 11)

project(voltmeter)
add_executable(${PROJECT_NAME} "main.cpp")
target_link_libraries(${PROJECT_NAME} m2k)
