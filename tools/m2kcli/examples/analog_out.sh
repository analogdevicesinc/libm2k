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

#calibrate the DAC
m2kcli analog-out auto --calibrate

#generate a cyclic signal based on the samples located in file.csv - csv format
m2kcli analog-out auto --generate channel=0 cyclic=1 raw=0 buffer_size=1024 < file.csv

#generate a cyclic signal based on the samples located in file.bin - binary format
m2kcli analog-out auto -9 channel=0,1 cyclic=1 raw=0 format=binary < file.bin
