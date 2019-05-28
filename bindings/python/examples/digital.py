import libm2k

ctx=libm2k.m2kOpen()
dig=ctx.getDigital()

dig.setSampleRateIn(10000)
dig.setSampleRateOut(10000)

for i in range(3):
    dig.setDirection(i,libm2k.DIO_OUTPUT)
    dig.enableChannel(i,True)

buff=range(8) # create 3 bit binary counter
dig.setCyclic(True)
dig.push(buff)

data = dig.getSamples(100)

for val in data:
    print(bin(val))
