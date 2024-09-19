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

import sys
import libm2k
import time
import os


def calibrate(ctx):
    # Calibrates ADC and DAC
    # Arguments:
    #    ctx  -- the available m2k context\n
    # Returns:
    #    adc_calib-- Must be True if the calibration was successful
    #    dac_calib--Must be True if the calibration was successful

    # ADC
    adc_calib = ctx.calibrateADC()
    # DAC
    dac_calib = ctx.calibrateDAC()
    return (adc_calib, dac_calib)


def open_context():
    #  Opens the context and retrieves analog objects
    #  Returns:
    #    ctx-- M2k context opened
    #    ain-- AnalogIn object
    #    aout--Analog Out object
    #    trig--Trigger object
    uri = "ip:192.168.2.1"
    if len(sys.argv) >= 3 and ("--uri" in sys.argv):
        for i in range(len(sys.argv)):
            if sys.argv[i] == "--uri":
                uri = sys.argv[i + 1]
                sys.argv.pop(i)
                sys.argv.pop(i)
                break
    print("URI " + uri)
    ctx = libm2k.m2kOpen(uri)
    if ctx is None:
        print("Connection Error: No available ADALM2000 found at the specified URI.")
        exit(1)
    # define input and output
    try:
        ain = ctx.getAnalogIn()
    except:
        ain = 0
    try:
        aout = ctx.getAnalogOut()
    except:
        aout = 0
    # define trigger
    try:
        trig = ain.getTrigger()
    except:
        trig = 0

    return ctx, ain, aout, trig


def create_dir(timestamp):
    # Creates a new directory where all the plot files will be saved
    # Arguments:
    #    timestamp -- Time and date when the program was run

    path = os.getcwd()

    results_dir = "results" + str(timestamp)
    path = os.path.join(path, results_dir)
    os.mkdir(path)

    return results_dir


# open context
ctx_timeout = 5000  # initial value of the timeout
ctx, ain, aout, trig = open_context()  # open context
ctx.setTimeout(ctx_timeout)  # set ctx timeout
if "ip:127.0.0.1" not in ctx.getUri():
    calibration = calibrate(ctx)
dig = ctx.getDigital()
d_trig = dig.getTrigger()
ps = ctx.getPowerSupply()
