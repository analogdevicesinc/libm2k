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
import struct

NB_OUT_SAMPLES = 1024

ctx=libm2k.m2kOpen()
if ctx is None:
	print("Connection Error: No ADALM2000 device available/connected to your PC.")
	exit(1)

ctx.calibrateADC()
ctx.calibrateDAC()

ain=ctx.getAnalogIn()
aout=ctx.getAnalogOut()
trig=ain.getTrigger()

ain.enableChannel(0,True)
ain.enableChannel(1,True)
ain.setSampleRate(100000)
ain.setRange(0,-10,10)
ain.setKernelBuffersCount(1)

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

x=np.linspace(-np.pi, np.pi, NB_OUT_SAMPLES)
buffer1=np.linspace(-3.0, 3.00, NB_OUT_SAMPLES)
buffer2=np.sin(x)

aout.setCyclic(True)

# Convert every voltage sample from the signal to its raw value.
# The library method expects a short* (C++), but in Python a bytearray is accepted.
# Change the type of the array from 'int64' to 'int16'.
buffer1_raw = np.array([aout.convertVoltsToRaw(0, item) for item in buffer1])
buffer1_raw = buffer1_raw.astype('int16')
buffer1_raw = bytearray(buffer1_raw)
buffer2_raw = np.array([aout.convertVoltsToRaw(1, item) for item in buffer2])
buffer2_raw = buffer2_raw.astype('int16')
buffer2_raw = bytearray(buffer2_raw)

aout.pushRawBytes(0, buffer1_raw, NB_OUT_SAMPLES)
aout.pushRawBytes(1, buffer2_raw, NB_OUT_SAMPLES)

for i in range(10):					# gets 10 triggered samples then quits
	ain.flushBuffer()
	data = ain.getSamplesRawInterleaved(1000)	# allows a memory view
	data = data.tobytes()				# convert the memory view to a readable bytearray
	count = int(len(data) / 2)

	data = struct.unpack('h'*count, data)		# interpret the raw interleaved samples as shorts
	data1 = data[0::2]
	data2 = data[1::2]

	# convert raw values to Volts for each channel
	data1 = [ain.convertRawToVolts(0, item) for item in data1]
	data2 = [ain.convertRawToVolts(1, item) for item in data2]
	print(data1)

	plt.plot(data1)
	plt.plot(data2)
	plt.show()
	time.sleep(0.1)

libm2k.contextClose(ctx)
