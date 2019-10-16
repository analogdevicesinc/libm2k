# This example reads the analog voltage from channel 0 of the analog input

import libm2k as l

channel = 0

ctx=l.m2kOpen()
if ctx is None:
	print("Connection Error: No ADALM2000 device available/connected to your PC.")
	exit(1)

ctx.calibrateADC()
ain=ctx.getAnalogIn()
ain.enableChannel(channel,True)
print(ain.getVoltage()[channel])


l.contextClose(ctx)
