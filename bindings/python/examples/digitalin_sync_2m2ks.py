#
# Copyright (c) 2019 Analog Devices Inc.
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

# Requirements: 2x ADALM2000
#
# This example assumes the following connections:
# DIO7 (1st M2K) -> DIO1 (1st M2K) -> DIO1 (2nd M2K)
# TO (1st M2K) -> TI (2nd M2K) -> DIO0 (2nd M2K)
#
# The application will generate a square wave on DIO7 of the first M2K. The signal is read on DIO1 pin of both modules.
# The acquisition on the 1st M2K is triggered by toggling the DIO0 pin. This is forwarded to the TI pin (2nd M2K) via
# the TO pin(1st M2K). This results in a synchronised acquisition on both devices.

import libm2k
import matplotlib.pyplot as plt
import time
import numpy as np

BUFFER_SIZE = 80000

DIGITAL_CH_TRIG = 0
DIGITAL_CH_READ = 1
DIGITAL_CH_PUSH = 7
SAMPLES_PER_PERIOD = 512
OFFSET = 5

sampling_frequency_in = 10000000
sampling_frequency_out = 750000


def generate_clock_signal(digital, channel, sampling_frequency):
    digital.setSampleRateOut(sampling_frequency)
    duty = SAMPLES_PER_PERIOD / 2  # 50%
    signal = np.arange(SAMPLES_PER_PERIOD) < duty
    buffer = list(map(lambda s: int(s) << channel, signal))
    for i in range(2):
        buffer.extend(buffer)
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

# Configure 1st M2K context
dig = ctx.getDigital()
dig.reset()
dig.setDirection(DIGITAL_CH_TRIG, libm2k.DIO_OUTPUT)  # DIO pin which the digital interface
dig.enableChannel(DIGITAL_CH_TRIG, False)
dig.setValueRaw(DIGITAL_CH_TRIG, libm2k.LOW)
dig.setDirection(DIGITAL_CH_READ, libm2k.DIO_INPUT)  # DIO pin on which the clock signal is read
dig.enableChannel(DIGITAL_CH_READ, False)
dig.setDirection(DIGITAL_CH_PUSH, libm2k.DIO_OUTPUT)  # DIO pin on which the clock signal is generated
dig.enableChannel(DIGITAL_CH_PUSH, True)
dig.setValueRaw(DIGITAL_CH_PUSH, libm2k.LOW)

# Configure trigger for 1st M2K context
trig = dig.getTrigger()
trig.reset()
trig.setDigitalDelay(0)  # Trigger is centered
trig.setDigitalSource(libm2k.SRC_NONE)  # Events on DigitalIn conditioned by internal trigger structure
trig.setDigitalCondition(DIGITAL_CH_TRIG, libm2k.RISING_EDGE_DIGITAL)  # Trigger condition
if trig.hasExternalTriggerOut():
    trig.setAnalogExternalOutSelect(libm2k.SELECT_DIGITAL_IN)  # Forward the digital trigger on TO pin

# Configure 2nd M2K context
dig2 = ctx2.getDigital()
dig2.reset()
dig2.setDirection(DIGITAL_CH_TRIG, libm2k.DIO_INPUT)  # DIO pin on which the trigger signal is read
dig2.enableChannel(DIGITAL_CH_TRIG, False)
dig2.setDirection(DIGITAL_CH_READ, libm2k.DIO_INPUT)  # DIO pin on which the clock signal is read
dig2.enableChannel(DIGITAL_CH_READ, False)

# Configure trigger for 2nd M2K context
trig2 = dig2.getTrigger()
trig2.reset()
trig2.setDigitalDelay(0)  # Trigger is centered
trig2.setDigitalSource(libm2k.SRC_TRIGGER_IN)  # Trigger events on TI trigger the DigitalIn interface
trig2.setDigitalExternalCondition(libm2k.RISING_EDGE_DIGITAL)  # Trigger condition

# Create and push buffer
buffer = generate_clock_signal(dig, DIGITAL_CH_PUSH, sampling_frequency_out)
dig.setCyclic(True)
dig.push(buffer)

dig.setSampleRateIn(sampling_frequency_in)
dig2.setSampleRateIn(sampling_frequency_in)

dig.startAcquisition(BUFFER_SIZE)
dig2.startAcquisition(BUFFER_SIZE)

# Toggle trigger pin
dig.setValueRaw(DIGITAL_CH_TRIG, libm2k.LOW)
dig.setValueRaw(DIGITAL_CH_TRIG, libm2k.HIGH)
dig.setValueRaw(DIGITAL_CH_TRIG, libm2k.LOW)

data = dig.getSamples(BUFFER_SIZE)
data2 = dig2.getSamples(BUFFER_SIZE)

# Extract data
digital_data_dio0 = list(map(lambda s: (((0x0001 << DIGITAL_CH_TRIG) & int(s)) >> DIGITAL_CH_TRIG) + 1.2, data))
digital_data_dio1 = list(map(lambda s: ((0x0001 << DIGITAL_CH_READ) & int(s)) >> DIGITAL_CH_READ, data))
digital_data2_dio0 = list(map(lambda s: (((0x0001 << DIGITAL_CH_TRIG) & int(s)) >> DIGITAL_CH_TRIG) + 1.2, data2))
digital_data2_dio1 = list(map(lambda s: ((0x0001 << DIGITAL_CH_READ) & int(s)) >> DIGITAL_CH_READ, data2))

# Plot routine
plt.plot(np.array(digital_data_dio0), label="Trigger Signal(1st M2K, DIO0)")
plt.plot(np.array(digital_data2_dio0) + OFFSET, label="Trigger Signal(2nd M2K, TI)")
plt.plot(np.array(digital_data_dio1) + 2 * OFFSET, label="Clock Signal(1st M2K)")
plt.plot(np.array(digital_data2_dio1) + 3 * OFFSET, label="Clock Signal(2nd M2K)")
plt.legend(loc="upper right")
plt.grid(True)
plt.show()
time.sleep(0.1)
libm2k.contextClose(ctx)
libm2k.contextClose(ctx2)
