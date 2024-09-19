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
import numpy as np
import time
from open_context import ain, ctx
import random
gen_reports = True


def config_for_ps_test(ps, ain):
    # Retrieve the Power supply object and enabe the power supply channels
    # Arguments:
    #    ps-- Power Supply object
    #    ain  -- AnalogIn object

    ctx.calibrateADC()
    # enable channels
    ps.enableChannel(libm2k.ANALOG_IN_CHANNEL_1, True)
    ps.enableChannel(libm2k.ANALOG_IN_CHANNEL_2, True)
    if not ain.isChannelEnabled(libm2k.ANALOG_IN_CHANNEL_1):
        ain.enableChannel(libm2k.ANALOG_IN_CHANNEL_1, True)
    if not ain.isChannelEnabled(libm2k.ANALOG_IN_CHANNEL_2):
        ain.enableChannel(libm2k.ANALOG_IN_CHANNEL_2, True)
    return


def ps_test_positive(ps, ain):
    # Tests the positive power supply
    # Arguments:
    #    ps -- Power Supply object
    #    ain -- AnalogIn object
    # Returns:
    #    pos_supply-- Vector that  holds 1 if the  voltage value read on the channel equals the voltage sent

    if gen_reports:
        from create_files import results_file, open_files_and_dirs
        if results_file is None:
            file, dir, csv = open_files_and_dirs()
        else:
            file = results_file
    else:
        file = []

    voltage = 0
    t = 0.1  # threshold value
    positive = True
    read_voltage_values = []
    sent_voltage_values = []
    pos_supply = []
    ain.setRange(libm2k.ANALOG_IN_CHANNEL_1, libm2k.PLUS_MINUS_25V)
    while voltage <= 5:
        ps.pushChannel(libm2k.ANALOG_IN_CHANNEL_1, voltage)
        time.sleep(0.2)
        read_voltage = (ain.getVoltage()[libm2k.ANALOG_IN_CHANNEL_1])
        read_voltage_values = np.append(read_voltage_values, read_voltage)
        sent_voltage_values = np.append(sent_voltage_values, voltage)
        if (voltage - t) <= read_voltage <= (voltage + t):
            pos_supply = np.append(pos_supply, 1)
        else:
            pos_supply = np.append(pos_supply, 0)
        voltage = voltage + random.uniform(0.4, 0.5)  # add a random value to the previous voltage value

    if gen_reports:
        write_file(file, read_voltage_values, sent_voltage_values, positive)
    return pos_supply


def ps_test_negative(ps, ain):
    # Tests the negativepower supply
    # Arguments:
    #    ps -- Power Supply object
    #    ain -- AnalogIn object
    # Returns:
    #    neg_supply-- Vector that  holds 1 if the  voltage value read on the channel equals the voltage sent

    if gen_reports:
        from create_files import results_file, open_files_and_dirs
        if results_file is None:
            file, dir, csv = open_files_and_dirs()
        else:
            file = results_file
    else:
        file = []
    voltage = 0
    positive = False
    neg_supply = []
    read_voltage_values = []
    sent_voltage_values = []
    t = 0.2  # threshold value
    ain.setRange(libm2k.ANALOG_IN_CHANNEL_2, libm2k.PLUS_MINUS_25V)
    while voltage >= -5:
        ps.pushChannel(libm2k.ANALOG_IN_CHANNEL_2, voltage)
        time.sleep(0.2)
        read_voltage = (ain.getVoltage()[libm2k.ANALOG_IN_CHANNEL_2])
        read_voltage_values = np.append(read_voltage_values, read_voltage)
        sent_voltage_values = np.append(sent_voltage_values, voltage)
        if (voltage + t) >= read_voltage >= (voltage - t):
            neg_supply = np.append(neg_supply, 1)
        else:
            neg_supply = np.append(neg_supply, 0)
        voltage = voltage - random.uniform(0.4, 0.5)  # subtract a random value from the previous voltage value

    if gen_reports:
        write_file(file, read_voltage_values, sent_voltage_values, positive)

    return neg_supply


def write_file(file, read_voltages, sent_voltages,positive ):
    if positive:
        file.write("\n\nPositive power supply test:\n")
    else:
        file.write("\n\nNegative power supply test:\n")
    for i in range(len(read_voltages)):
        file.write("Sent voltage: " + str(sent_voltages[i]) + "\n")
        file.write("Read voltage: " + str(read_voltages[i]) + "\n")