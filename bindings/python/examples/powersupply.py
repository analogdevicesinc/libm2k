# This exammple assumes connection:
# 1+ to V+
# 1- to GND
# This example turns on the power supply and sets it to 1.7V and then reads back the value

import libm2k

ctx=libm2k.m2kOpen()
if ctx is None:
	print("Connection Error: No ADALM2000 device available/connected to your PC.")
	exit(1)

ctx.calibrateADC()
ps=ctx.getPowerSupply()
ps.enableChannel(0,True)
ps.pushChannel(0,1.7)
ain=ctx.getAnalogIn()
ain.enableChannel(0,True)
print(ain.getVoltage()[0])

libm2k.contextClose(ctx)
