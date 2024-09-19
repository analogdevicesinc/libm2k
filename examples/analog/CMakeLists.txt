#
# Copyright (c) 2024 Analog Devices Inc.
#
# This file is part of libm2k
# (see http://www.github.com/analogdevicesinc/libm2k).
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 2.1 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#

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
add_executable(triggered_aout "triggered_aout.cpp")

target_link_libraries(analog_in_out PRIVATE libm2k::libm2k)
target_link_libraries(streaming_one_channel PRIVATE libm2k::libm2k)
target_link_libraries(streaming_synchronized PRIVATE libm2k::libm2k)
target_link_libraries(sync_stream_diff_frequencies PRIVATE libm2k::libm2k)
target_link_libraries(stream_test_adc PRIVATE libm2k::libm2k)
target_link_libraries(stream_test_dac PRIVATE libm2k::libm2k)
target_link_libraries(analog_out_set_voltage PRIVATE libm2k::libm2k)
target_link_libraries(triggered_aout PRIVATE libm2k::libm2k)

if (PTHREAD_LIBRARIES)
	target_link_libraries(sync_stream_diff_frequencies PRIVATE ${PTHREAD_LIBRARIES})
	target_link_libraries(stream_test_adc PRIVATE ${PTHREAD_LIBRARIES})
	target_link_libraries(stream_test_dac PRIVATE ${PTHREAD_LIBRARIES})
endif()




