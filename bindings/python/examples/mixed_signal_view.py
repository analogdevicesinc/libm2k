#
# Copyright (c) 2020 Analog Devices Inc.
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
# DIO_0 -> 1+
# GND -> 1-
#
# The application will generate a square on DIO_0. The signal is fed back into the analog input and digital input
# and the voltage values are displayed on the screen


import libm2k
import numpy as np
import matplotlib.pyplot as plt

DIGITAL_CH = 0
ANALOG_CH = 0

SAMPLING_FREQUENCY = 10000000
OVERSAMPLING = 1

BUFFER_SIZE = 1600
SAMPLES_PER_PERIOD = 16


def generate_clock_signal(digital, sampling_frequency):
    digital.setSampleRateOut(sampling_frequency)
    duty = SAMPLES_PER_PERIOD / 2  # 50%
    signal = np.arange(SAMPLES_PER_PERIOD) < duty
    buffer = list(map(lambda s: int(s) << DIGITAL_CH, signal))
    for i in range(8):
        buffer.extend(buffer)
    digital.push(buffer)


def main():
    context = libm2k.m2kOpen()
    if context is None:
        print("Connection Error: No ADALM2000 device available/connected to your PC.")
        exit(1)

    # check if mixed signal is available on your firmware version
    if context.hasMixedSignal() is False:
        print("Mixed Signal not available")
        exit(1)

    context.calibrateDAC()

    analog_in = context.getAnalogIn()
    digital = context.getDigital()
    trigger = analog_in.getTrigger()

    analog_in.enableChannel(libm2k.CHANNEL_1, True)
    analog_in.enableChannel(libm2k.CHANNEL_2, True)

    # configure the trigger
    trigger.setAnalogDelay(-20)
    trigger.setDigitalDelay(-20)
    trigger.setAnalogLevel(ANALOG_CH, 2)
    trigger.setDigitalCondition(DIGITAL_CH, libm2k.NONE)
    trigger.setAnalogSource(ANALOG_CH)
    trigger.setAnalogMode(ANALOG_CH, libm2k.ANALOG)
    trigger.setAnalogCondition(ANALOG_CH, libm2k.RISING_EDGE_ANALOG)

    # configure digital out interface
    digital.setOutputMode(DIGITAL_CH, libm2k.DIO_PUSHPULL)
    digital.setDirection(DIGITAL_CH, libm2k.DIO_OUTPUT)
    digital.enableChannel(DIGITAL_CH, True)
    digital.setCyclic(False)

    # use the same samplerate for both analog and digital interface
    analog_in.setSampleRate(SAMPLING_FREQUENCY)
    analog_in.setOversamplingRatio(OVERSAMPLING)
    digital.setSampleRateIn(SAMPLING_FREQUENCY // OVERSAMPLING)

    # startMixedSignal -> extract the data -> stopMixedSignal
    context.startMixedSignalAcquisition(BUFFER_SIZE)
    generate_clock_signal(digital, SAMPLING_FREQUENCY // (10 * OVERSAMPLING))
    digital_data, analog_data = [], []
    digital_data.extend(digital.getSamples(BUFFER_SIZE))
    analog_data.extend(analog_in.getSamples(BUFFER_SIZE))
    context.stopMixedSignalAcquisition()

    # get only the used digital channel
    digital_data_one_ch = list(map(lambda s: ((0x0001 << DIGITAL_CH) & int(s)) >> DIGITAL_CH, digital_data))
    plt.plot(digital_data_one_ch, label='Digital')
    plt.plot(analog_data[ANALOG_CH], label='Analog')
    plt.show()

    libm2k.contextClose(context, True)

if __name__ == '__main__':
    main()
