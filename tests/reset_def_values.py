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

import libm2k


# set default values for analogin
def analog_in(ain):
    #  Sets default values for ain parameters
    # Arguments:
    #    ain  -- AnalogIn object

    ain.stopAcquisition()
    ain.enableChannel(libm2k.ANALOG_IN_CHANNEL_1, True)
    ain.enableChannel(libm2k.ANALOG_IN_CHANNEL_2, True)
    ain.setVerticalOffset(libm2k.ANALOG_IN_CHANNEL_1, 0)
    ain.setVerticalOffset(libm2k.ANALOG_IN_CHANNEL_2, 0)
    ain.setRange(libm2k.ANALOG_IN_CHANNEL_1, libm2k.PLUS_MINUS_2_5V)
    ain.setRange(libm2k.ANALOG_IN_CHANNEL_2, libm2k.PLUS_MINUS_2_5V)
    ain.setOversamplingRatio(1)
    ain.setSampleRate(100000)
    ain.setKernelBuffersCount(1)

    return


# set default values for analog out
def analog_out(aout):
    # Sets default values for aout parameters
    # Arguments:
    #    aout  -- AnalogOut object

    aout.enableChannel(libm2k.ANALOG_IN_CHANNEL_1, True)
    aout.enableChannel(libm2k.ANALOG_IN_CHANNEL_2, True)
    aout.setSampleRate(libm2k.ANALOG_IN_CHANNEL_1, 750000)
    aout.setSampleRate(libm2k.ANALOG_IN_CHANNEL_2, 750000)
    aout.setOversamplingRatio(libm2k.ANALOG_IN_CHANNEL_1, 1)
    aout.setOversamplingRatio(libm2k.ANALOG_IN_CHANNEL_2, 1)
    aout.setCyclic(True)
    return


# set default values for hardware trigger
def trigger(trig):
    # Sets default values for trigger parameters
    # Arguments:
    #    trig  -- Trigger object

    trig.setAnalogLevel(libm2k.ANALOG_IN_CHANNEL_1, 0.0)
    trig.setAnalogLevel(libm2k.ANALOG_IN_CHANNEL_2, 0.0)
    trig.setAnalogHysteresis(libm2k.ANALOG_IN_CHANNEL_1, 0)
    trig.setAnalogHysteresis(libm2k.ANALOG_IN_CHANNEL_2, 0)
    trig.setAnalogCondition(libm2k.ANALOG_IN_CHANNEL_1, libm2k.RISING_EDGE_ANALOG)
    trig.setAnalogCondition(libm2k.ANALOG_IN_CHANNEL_2, libm2k.RISING_EDGE_ANALOG)
    trig.setAnalogMode(libm2k.ANALOG_IN_CHANNEL_1, libm2k.ALWAYS)
    trig.setAnalogMode(libm2k.ANALOG_IN_CHANNEL_1, libm2k.ALWAYS)
    trig.setAnalogSource(libm2k.CHANNEL_1)
    trig.setAnalogSourceChannel(libm2k.ANALOG_IN_CHANNEL_1)
    trig.setAnalogDelay(0)

    return


def digital(dig: libm2k.M2kDigital):
    # Sets default values for digital parameters
    # Arguments:
    #    dig  -- Digital object

    dig.setSampleRateIn(10000)
    dig.setSampleRateOut(10000)
    dig.setCyclic(True)
    dig.setDirection(0b1111111111111111)
    for i in range(16):
        dig.setOutputMode(i, 1)
    dig.enableAllOut(True)
