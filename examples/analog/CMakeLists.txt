cmake_minimum_required(VERSION 3.1.3)
set (CMAKE_CXX_STANDARD 11)
project(analog LANGUAGES CXX)


if (NOT WIN32)
	find_library(PTHREAD_LIBRARIES pthread)
endif()

if (LIBM2K_VERSION)			# in-source build
	set(PROJECT_VERSION ${LIBM2K_VERSION})
else()					# standalone build
	set(PROJECT_VERSION "1.0.0")
	find_package(libm2k REQUIRED)
endif()

add_executable(analog_in_out "analog_in_out.cpp")
add_executable(streaming_one_channel "streaming_one_channel.cpp")
add_executable(streaming_synchronized "streaming_synchronized.cpp")
add_executable(sync_stream_diff_frequencies "sync_stream_diff_frequencies.cpp")
add_executable(stream_test_adc "stream_test_adc.cpp")
add_executable(stream_test_dac "stream_test_dac.cpp")
add_executable(analog_out_set_voltage "analog_out_set_voltage.cpp")

target_link_libraries(analog_in_out PRIVATE libm2k::libm2k)
target_link_libraries(streaming_one_channel PRIVATE libm2k::libm2k)
target_link_libraries(streaming_synchronized PRIVATE libm2k::libm2k)
target_link_libraries(sync_stream_diff_frequencies PRIVATE libm2k::libm2k)
target_link_libraries(stream_test_adc PRIVATE libm2k::libm2k)
target_link_libraries(stream_test_dac PRIVATE libm2k::libm2k)
target_link_libraries(analog_out_set_voltage PRIVATE libm2k::libm2k)

if (PTHREAD_LIBRARIES)
	target_link_libraries(sync_stream_diff_frequencies PRIVATE ${PTHREAD_LIBRARIES})
	target_link_libraries(stream_test_adc PRIVATE ${PTHREAD_LIBRARIES})
	target_link_libraries(stream_test_dac PRIVATE ${PTHREAD_LIBRARIES})
endif()




