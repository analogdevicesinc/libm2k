#
# Copyright (c) 2025 Analog Devices Inc.
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
"""
Requirements: x1 ADALM2000

This example assumes the following connections:
    W1      ->    1+
    W2      ->    2+
    GND     ->    1-
    GND     ->    2-
"""

import libm2k
import numpy as np
import matplotlib.pyplot as plt

# Configuration
URI = "ip:192.168.2.1"

ADC_SR = 100_000_000  # [Hz]
TRIGGER_SOURCE = libm2k.ANALOG_IN_CHANNEL_1
DELAY = -2000 # [samples]
INPUT_BUFFER_SIZE = 7_000

# NOTE: Period = N_SAMPLES / DAC_SR
DAC_SR = 75_000_000  # [Hz]
N_SAMPLES = 2048

AMPLITUDE = 2  # [V]
OFFSET = 0  # [V]


def main():
    try:
        context: libm2k.M2k = libm2k.m2kOpen(URI)
        if context is None:
            print("Connection Error: No ADALM2000 device available/connected to your PC.")
            exit(1)

        ain: libm2k.M2kAnalogIn = context.getAnalogIn()
        aout: libm2k.M2kAnalogOut = context.getAnalogOut()
        trig: libm2k.M2kHardwareTrigger = ain.getTrigger()

        # Prevent bad initial configuration
        context.reset()
        context.calibrateADC()
        context.calibrateDAC()

        # Configure both input channels
        ain.setSampleRate(ADC_SR)
        ain.enableChannel(libm2k.ANALOG_IN_CHANNEL_1, True)
        ain.enableChannel(libm2k.ANALOG_IN_CHANNEL_2, True)
        # NOTE: decreased range increases the resolution
        ain.setRange(libm2k.ANALOG_IN_CHANNEL_1, libm2k.PLUS_MINUS_2_5V)
        ain.setRange(libm2k.ANALOG_IN_CHANNEL_2, libm2k.PLUS_MINUS_2_5V)

        # Configure both output channels
        aout.setSampleRate(0, DAC_SR)
        aout.setSampleRate(1, DAC_SR)
        aout.enableChannel(0, True)
        aout.enableChannel(1, True)

        # NOTE: non-cyclic will generate a single sweep
        aout.setCyclic(False)

        signal = OFFSET + AMPLITUDE * np.linspace(-1, 1, N_SAMPLES)

        # Configure the trigger source for signal acquisition
        trig.setAnalogSource(TRIGGER_SOURCE)
        trig.setAnalogCondition(TRIGGER_SOURCE, libm2k.RISING_EDGE_ANALOG)
        trig.setAnalogLevel(libm2k.ANALOG_IN_CHANNEL_1, -(OFFSET + AMPLITUDE) / 2)
        trig.setAnalogDelay(DELAY)
        trig.setAnalogMode(TRIGGER_SOURCE, libm2k.ANALOG)

        print("Starting signal acquisition...")
        ain.startAcquisition(INPUT_BUFFER_SIZE)

        print("Generating DC Sweep...")
        aout.push([signal, signal])

        # Visualize the results
        data = np.array(ain.getSamples(INPUT_BUFFER_SIZE))
        ain0 = data[0, :]
        ain1 = data[1, :]

        plt.figure()
        plt.plot(ain0, label="AIN0", color="orange")
        plt.xlabel("Sample")
        plt.ylabel("Voltage [V]")
        plt.title("DC Sweep AIN0")
        plt.legend(loc="upper right")
        plt.grid(True)
        plt.show()

        plt.figure()
        plt.plot(ain1, label="AIN1", color="blue")
        plt.xlabel("Sample")
        plt.ylabel("Voltage [V]")
        plt.title("DC Sweep AIN1")
        plt.legend(loc="upper right")
        plt.grid(True)
        plt.show()

    except Exception as e:
        print("An error occurred:", e)

    libm2k.contextClose(context)


if __name__ == "__main__":
    main()
