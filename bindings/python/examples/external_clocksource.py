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

# This example assumes the following connection:
# W1 -> DIO0
#
# The application will generate a square wave on W1 which is fed into DIO0
# and serves as external clock signal for the digital device. A pattern generated
# on DIO1 is plotted and displayed.

import libm2k
import numpy as np
import matplotlib.pyplot as plt

max_dac_sample_rate = 75000000
number_of_samples = 1024
clock_frequency = 75000000
digital_sampling_frequency = 100000000
samples_per_period = 1024
digital_ch = 1
clock_ch = 0
acquisition_buffer_size = 4096


def generate_clock_signal(number_of_samples):

    buffer = []
    for i in range(int(number_of_samples/2)):
        buffer.append(0)
    for i in range(int(number_of_samples/2)):
        buffer.append(3.3)
    print(buffer)
    return buffer


# Context setup

ctx = libm2k.m2kOpen()
aout = ctx.getAnalogOut()
dig = ctx.getDigital()

# Prevent bad initial config
dig.reset()
aout.reset()

ctx.calibrateDAC()
ctx.calibrateADC()

# AnalogOut setup

aout.setCyclic(True)
aout.enableChannel(clock_ch, True)
aout.setSampleRate(clock_ch, max_dac_sample_rate)
oversampling_ratio = max_dac_sample_rate/(clock_frequency * number_of_samples)
aout.setOversamplingRatio(clock_ch, int(oversampling_ratio))
buffer = generate_clock_signal(number_of_samples)
aout.push(clock_ch, buffer)

# Digital Setup

dig.reset()
dig.setExternalClocksource(True)
print("Clocksource external --> "+str(dig.isClocksourceExternal()))
dig.setCyclic(True)
dig.setSampleRateIn(digital_sampling_frequency)
dig.setDirection(clock_ch, libm2k.DIO_INPUT)
dig.enableChannel(clock_ch, False)
dig.setDirection(digital_ch, libm2k.DIO_OUTPUT)
dig.setSampleRateOut(digital_sampling_frequency)
dig.enableChannel(digital_ch, True)

duty = samples_per_period / 2  # 50%
signal = np.arange(samples_per_period) < duty
digital_buffer = list(map(lambda s: int(s) << digital_ch, signal))  # generate signal on desired channel

for i in range(8):
    buffer.extend(buffer)

dig.push(digital_buffer)

digital_data = dig.getSamples(acquisition_buffer_size)

digital_data_one_ch = list(map(lambda s: ((0x0001 << digital_ch) & int(s)) >> digital_ch, digital_data))
plt.plot(digital_data_one_ch, label='Digital')
plt.show()


