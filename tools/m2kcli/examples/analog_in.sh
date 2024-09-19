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

#capture 2048 processed samples from both channels - csv format
m2kcli analog-in auto --capture channel=0,1 buffer_size=1024 nb_samples=2048 raw=0

#equivalent to the command above
m2kcli analog-in auto -c buffer_size=1024 nb_samples=2048 channel=0,1 raw=0 format=csv

#capture 100 unprocessed samples from both channels and save them into a binary file
m2kcli analog-in auto --capture channel=0,1 buffer_size=100 nb_samples=100 raw=1 format=binary > file.bin

#get the voltage, unprocessed, for the first channel
m2kcli analog-in auto --voltage channel=0 raw=0

#get the voltage for both channels as raw values
m2kcli analog-in auto -v channel=0,1 raw=1

#get all global attributes
m2kcli analog-in auto --get all

#get some attributes
m2kcli analog-in auto -g sampling_frequency trigger_delay

#get the value of the trigger_mode for both channels
m2kcli analog-in auto --get-channel channel=0,1 trigger_mode

#set sampling_frequency and trigger_source
m2kcli analog-in auto --set sampling_frequency=100000 trigger_source=channel_1

#set the range and the trigger_mode value
m2kcli analog-in auto --set-channel channel=1 range=high trigger_mode=analog
