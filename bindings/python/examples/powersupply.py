import libm2k

ctx=libm2k.m2kOpen()
ctx.calibrateADC()
ps=ctx.getPowerSupply()
ps.enableChannel(0,True)
ps.pushChannel(0,1.7)
ain=ctx.getAnalogIn()
ain.enableChannel(0,True)
print(ain.getVoltage()[0])

libm2k.deviceClose(ctx)