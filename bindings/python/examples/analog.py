import libm2k
import matplotlib.pyplot as plt
import time
import numpy as np

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

trig.setAnalogSource(0) # Channel 0 as source
trig.setAnalogCondition(0,libm2k.RISING_EDGE)
trig.setAnalogLevel(0,0.5)  # Set trigger level at 0.5
trig.setAnalogDelay(0) # Trigger is centered
trig.setAnalogMode(1, libm2k.ANALOG)

aout.setSampleRate(0, 750000)
aout.setSampleRate(1, 750000)
aout.enableChannel(0, True)
aout.enableChannel(1, True)

x=np.linspace(-np.pi,np.pi,1024)
buffer1=np.linspace(-2.0,2.00,1024)
buffer2=np.sin(x)

buffer = [buffer1, buffer2]

aout.setCyclic(True)
aout.push(buffer)

for i in range(10): # gets 10 triggered samples then quits
    data = ain.getSamples(1000)
    plt.plot(data[0])
    plt.plot(data[1])
    plt.show()
    time.sleep(0.1)

libm2k.deviceClose(ctx)
