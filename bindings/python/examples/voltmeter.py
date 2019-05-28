import libm2k as l

channel = 0

ctx=l.m2kOpen()
ctx.calibrateADC()
ain=ctx.getAnalogIn()
ain.enableChannel(channel,True)
print(ain.getVoltage()[channel])


l.deviceClose(ctx)
