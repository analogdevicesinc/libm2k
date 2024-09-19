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

#Please check Electronics Lab 14 on our wiki page: https://wiki.analog.com/university/courses/electronics/electronics-lab-14
#Configure the connections for 'Unipolar output operation' mode

m2kcli spi auto -i frequency=1000000 clk=1 mosi=2 miso=7 bit_numbering=MSB cs=0 mode=3 cs_polarity=0 -w data=0x09,0xC4
