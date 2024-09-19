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

#capture 1000 samples - binary format
m2kcli digital auto --capture buffer_size=100 nb_samples=1000 format=binary

#continuously samples capturing - csv format
m2kcli digital auto --capture buffer_size=1024

#generate a cyclic signal for the given channels based on the samples located in file.csv
m2kcli digital auto --generate channel=0,1,4,7,13,15 cyclic=1 < file.csv

#generate a non-cyclic signal for the given channels based on the samples gave as input
m2kcli digital auto --generate channel=1,2,3,4,5 cyclic=0 format=binary
# ~$1000
# ~$1000
#  ...
# ~$1000
# ~$1000
# ~$CTRL + C