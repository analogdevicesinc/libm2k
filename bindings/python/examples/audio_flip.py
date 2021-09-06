#
# Copyright (c) 2019 Analog Devices Inc.
#
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
# W1 -> 100-ohm resistor in series with speaker
# W2 -> open
# 2+ -> Microphone
# GND -> 1-
# GND -> 2-
#
# The application will record a few seconds of audio on scope channel 2, reverse the order of the samples,
# Apply some filtering tricks to remove large DC offsets, then play back. Playback is at 75% of record speed just due
# to available sample rates.

import libm2k
import matplotlib.pyplot as plt
import time
import numpy as np

ctx = libm2k.m2kOpen()
if ctx is None:
    print("Connection Error: No ADALM2000 device available/connected to your PC.")
    exit(1)

ain = ctx.getAnalogIn()
aout = ctx.getAnalogOut()
trig = ain.getTrigger()
ps = ctx.getPowerSupply()

# Prevent bad initial config for ADC and DAC
ain.reset()
aout.reset()
ps.reset()

ctx.calibrateADC()
ctx.calibrateDAC()

ps.enableChannel(0, True)  # Consider using for microphone power,
ps.pushChannel(0, 4.0)  # unfortunately DC blocking cap takes forever to charge
print("waiting 1 second for mic to settle...")
time.sleep(1.0)

ain.enableChannel(0, True)
ain.enableChannel(1, True)
ain.setSampleRate(10000)
ain.setRange(1, -1, 1)

print("ain sample rate: " + str(ain.getSampleRate()))
print("ain range: " + str(ain.getRangeLimits(1)))
input("\nPress key to record")
print("Recording...")
time.sleep(0.1)

startime = time.time()

bufflen = 2 ** 16

data = ain.getSamples(bufflen)

captime = time.time() - startime
print("recorded time: " + str(captime))

print("playing back flipped...")

aout.setSampleRate(0, 7500)
aout.setSampleRate(1, 7500)
aout.enableChannel(0, True)
aout.enableChannel(1, True)

print("aout sample rate: " + str(aout.getSampleRate()))
audio = data[1]
gain = 100.0
dc = np.convolve(audio, (np.ones(64) / 64.0), mode='same')  # Calculate running DC average
audio = audio - dc
audio[0:100] = [0] * 100  # get rid of edge effect
normal_audio = audio
audio = np.flip(audio)  # Here's where the flip happens!!
audio = audio * gain  # add some gain

# Push data
buffer = [audio, audio]
aout.setCyclic(True)
aout.push(buffer)

# Plot audio data
plt.plot(audio, label="flipped audio")
plt.plot(np.array(normal_audio * gain), label="normal audio")
plt.legend()
plt.show()
time.sleep(5)

libm2k.contextClose(ctx)
