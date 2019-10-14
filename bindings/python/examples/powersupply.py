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
