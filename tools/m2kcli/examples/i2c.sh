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

#  This example uses EVAL-ADT7420-PMDZ as a slave
#  Hardware configuration:
#  	(ADALM2000) DIO_0 <---> Pin 1 (ADT7420) <--->  10 kilohms resistor <--- V+ (ADALM2000)
#  	(ADALM2000) DIO_1 <---> Pin 3 (ADT7420) <---> 10 kilohms resistor <--- V+ (ADALM2000)
#  	(ADALM2000) GND <---> Pin 5 (ADT7420)
#  	(ADALM2000) V+ ---> Pin 7 (ADT7420)

m2kcli i2c ip:192.168.2.1 -i frequency=100000 address=0x48 scl=0 sda=1 -w data=0x0B option=3
m2kcli i2c ip:192.168.2.1 -i frequency=100000 address=0x48 scl=0 sda=1 -r bytes_number=1 option=1
m2kcli i2c ip:192.168.2.1 -i frequency=100000 address=0x48 scl=0 sda=1 -w data=0 option=3
m2kcli i2c ip:192.168.2.1 -i frequency=100000 address=0x48 scl=0 sda=1 -r bytes_number=2 option=1
