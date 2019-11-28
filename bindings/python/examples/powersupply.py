#
# Copyright (c) 2019 Analog Devices Inc.
#
# This file is part of libm2k
# (see http://www.github.com/analogdevicesinc/libm2k).
#
# This program is free software: you can redistribute it and/or modify
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

# This exammple assumes connection:
# 1+ to V+
# 1- to GND
# This example turns on the power supply and sets it to 1.7V and then reads back the value

import libm2k

ctx=libm2k.m2kOpen()
if ctx is None:
	print("Connection Error: No ADALM2000 device available/connected to your PC.")
	exit(1)

ctx.calibrateADC()
ps=ctx.getPowerSupply()
ps.enableChannel(0,True)
ps.pushChannel(0,1.7)
ain=ctx.getAnalogIn()
ain.enableChannel(0,True)
print(ain.getVoltage()[0])

libm2k.contextClose(ctx)
