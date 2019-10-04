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


l.deviceClose(ctx)
