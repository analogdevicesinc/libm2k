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
# Requirements: 2x ADALM2000
#
# This example assumes the following connections:
# W1 (1st M2K) -> 1+ (1st M2K) -> 1+ (2nd M2K)
# DIO0 (1st M2K) -> 2+ (1st M2K)
# TO (1st M2K) -> TI (2nd M2K) -> 2+ (2nd M2K)
# GND (1st M2K) -> GND (2nd M2K)
# The application will generate a square wave on W1 of the first M2K. The signal is read on Analog Input 1 of both M2Ks.
# The acquisition on the 1st M2K is triggered by toggling the DIO0 pin. This is forwarded to the TI pin (2nd M2K) via
# the TO pin(1st M2K). This results in a synchronised acquisition on both devices. The trigger can be observed on
# Analog Input 2 of both modules.

import libm2k
import matplotlib.pyplot as plt
import time
import numpy as np

DIGITAL_CH = 0

BUFFER_SIZE = 800000
AMPLITUDE = 4
SAMPLES_PER_PERIOD = 256
PLOTTED_SAMPLES = BUFFER_SIZE
OFFSET = 5

sampling_frequency_in = 10000000
sampling_frequency_out = 750000


def generate_clock_signal():
    buffer = []
    for i in range(256):
        buffer.append(1)
    for i in range(256):
        buffer.append(0)

    for i in range(4):
        buffer.extend(buffer)
    buffer = buffer * AMPLITUDE
    print(buffer)
    return buffer


ctx = libm2k.m2kOpen("replace with uri for 1st M2K")
ctx2 = libm2k.m2kOpen("replace with uri for 2nd M2K")

if ctx is None:
    print("Connection Error: No ADALM2000 device available/connected to your PC.")
    exit(1)

if ctx2 is None:
    print("Connection Error: No second ADALM2000 device available/connected to your PC.")
    exit(1)

ctx.calibrateADC()
ctx.calibrateDAC()
ctx2.calibrateADC()
ctx2.calibrateDAC()

# Configure 1st context
ain = ctx.getAnalogIn()
aout = ctx.getAnalogOut()
dig = ctx.getDigital()
trig = ain.getTrigger()

if trig.hasExternalTriggerOut():
    trig.setAnalogExternalOutSelect(libm2k.SELECT_ANALOG_IN)  # Forward Analog trigger on TO pin

ain.enableChannel(0, True)
ain.enableChannel(1, True)
ain.setSampleRate(sampling_frequency_in)
ain.setRange(0, libm2k.PLUS_MINUS_2_5V)
ain.setRange(1, libm2k.PLUS_MINUS_2_5V)
dig.setDirection(DIGITAL_CH, libm2k.DIO_OUTPUT)
dig.enableChannel(DIGITAL_CH, True)
dig.setValueRaw(DIGITAL_CH, libm2k.LOW)

# Configure 2nd context
ain2 = ctx2.getAnalogIn()
dig2 = ctx2.getDigital()
trig2 = dig2.getTrigger()
ain2.enableChannel(0, True)
ain2.enableChannel(1, True)
ain2.setSampleRate(sampling_frequency_in)
ain2.setRange(0, libm2k.PLUS_MINUS_2_5V)
ain2.setRange(1, libm2k.PLUS_MINUS_2_5V)

# Trigger settings for 1st context
trig.setAnalogSource(1)  # Channel 2 as source
trig.setAnalogCondition(1, libm2k.RISING_EDGE_ANALOG)
trig.setAnalogLevel(1, 0.5)  # Set trigger level at 0.5
trig.setAnalogDelay(0)  # Trigger is centered
trig.setAnalogMode(1, libm2k.ANALOG)  # Trigger condition specified only by analog trigger

# Trigger settings for 2nd context
trig2.setAnalogMode(0, libm2k.EXTERNAL)  # Trigger condition specified only by external trigger (TI)
trig2.setAnalogSource(0)
trig2.setAnalogExternalCondition(0, libm2k.RISING_EDGE_ANALOG)
trig2.setAnalogLevel(0, 0.5)  # Set trigger level at 0.5
trig2.setAnalogDelay(0)  # Trigger is centered

# Configure analog out 1st context
aout.setSampleRate(0, sampling_frequency_out)
aout.setSampleRate(1, sampling_frequency_out)
aout.enableChannel(0, True)
aout.enableChannel(1, True)
aout.setCyclic(False)

ain.startAcquisition(BUFFER_SIZE)
ain2.startAcquisition(BUFFER_SIZE)

# Toggle trigger pin
dig.setValueRaw(DIGITAL_CH, libm2k.LOW)
dig.setValueRaw(DIGITAL_CH, libm2k.HIGH)

buffer = generate_clock_signal()
aout.push([buffer, buffer])

data = ain.getSamples(BUFFER_SIZE)
data2 = ain2.getSamples(BUFFER_SIZE)

# Plot routine
plt.plot(np.array(data[0]), label="Clock Signal(1st M2K)")
plt.plot(np.array(data2[0]) + OFFSET, label="Clock Signal(2nd M2K)")
plt.plot(np.array(data[1]) + 2 * OFFSET, label="Trigger Signal(1st M2K, DIO0)")
plt.plot(np.array(data2[1]) + 3 * OFFSET, label="Trigger Signal(2nd M2K, TI)")
plt.legend(loc="upper right")
plt.grid(True)
plt.show()
time.sleep(0.1)
aout.stop()
libm2k.contextClose(ctx)
libm2k.contextClose(ctx2)
