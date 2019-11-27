cmake_minimum_required(VERSION 3.1.3)
set (CMAKE_CXX_STANDARD 11)

project(analog LANGUAGES CXX VERSION ${LIBM2K_VERSION})
include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${IIO_INCLUDE_DIR}
	${CMAKE_SOURCE_DIR}/include
)
add_executable(analog_in_out "analog_in_out.cpp")
add_executable(streaming_one_channel "streaming_one_channel.cpp")
add_executable(streaming_synchronized "streaming_synchronized.cpp")
add_executable(sync_stream_diff_frequencies "sync_stream_diff_frequencies.cpp")


target_link_libraries(analog_in_out libm2k)
target_link_libraries(streaming_one_channel libm2k)
target_link_libraries(streaming_synchronized libm2k)
target_link_libraries(sync_stream_diff_frequencies libm2k)



