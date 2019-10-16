import libm2k

n_bits=4

ctx=libm2k.m2kOpen()
if ctx is None:
	print("Connection Error: No ADALM2000 device available/connected to your PC.")
	exit(1)

dig=ctx.getDigital()

dig.setSampleRateIn(10000)
dig.setSampleRateOut(10000)

for i in range(n_bits):
    dig.setDirection(i,libm2k.DIO_OUTPUT)
    dig.enableChannel(i,True)

buff=range(2**n_bits) # create 3 bit binary counter
dig.setCyclic(True)
dig.push(buff)

data = dig.getSamples(100)

for val in data:
    print(bin(val))
