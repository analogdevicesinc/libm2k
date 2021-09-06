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

# This example assumes the following connections:
# W1 -> 1+
# W2 -> 2+
# GND -> 1-
# GND -> 2-
#
# The application will generate a sine and triangular wave on W1 and W2. The signal is fed back into the analog input
# and the voltage values are displayed on the screen

import libm2k
import matplotlib.pyplot as plt
import time
import numpy as np

ctx=libm2k.m2kOpen()
if ctx is None:
	print("Connection Error: No ADALM2000 device available/connected to your PC.")
	exit(1)


ain=ctx.getAnalogIn()
aout=ctx.getAnalogOut()
trig=ain.getTrigger()

# Prevent bad initial config for ADC and DAC
ain.reset()
aout.reset()

ctx.calibrateADC()
ctx.calibrateDAC()

ain.enableChannel(0,True)
ain.enableChannel(1,True)
ain.setSampleRate(100000)
ain.setRange(0,-10,10)

### uncomment the following block to enable triggering
#trig.setAnalogSource(0) # Channel 0 as source
#trig.setAnalogCondition(0,libm2k.RISING_EDGE_ANALOG)
#trig.setAnalogLevel(0,0.5)  # Set trigger level at 0.5
#trig.setAnalogDelay(0) # Trigger is centered
#trig.setAnalogMode(1, libm2k.ANALOG)

aout.setSampleRate(0, 750000)
aout.setSampleRate(1, 750000)
aout.enableChannel(0, True)
aout.enableChannel(1, True)

x=np.linspace(-np.pi,np.pi,1024)
buffer1=np.linspace(-2.0,2.00,1024)
buffer2=np.sin(x)

buffer = [buffer1, buffer2]

aout.setCyclic(True)
aout.push(buffer)

for i in range(10): # gets 10 triggered samples then quits
    data = ain.getSamples(1000)
    plt.plot(data[0])
    plt.plot(data[1])
    plt.show()
    time.sleep(0.1)

libm2k.contextClose(ctx)
