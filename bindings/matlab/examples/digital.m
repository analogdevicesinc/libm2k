%% Setup
m2k = clib.libm2k.libm2k.devices.m2kOpen();

dig = m2k.getDigital();

dig.setSampleRateIn(100000);
dig.setSampleRateOut(100000);

N_BITS = 4;
DIO_OUTPUT = clib.libm2k.libm2k.digital.DIO_DIRECTION.DIO_OUTPUT;

for k=1:N_BITS
    dig.setDirection(k-1,DIO_OUTPUT);
    dig.enableChannel(k-1,true);
end

out = int16(0:N_BITS*2-1);

dig.setCyclic(true);
dig.push_raw(out)

bufferIn = dig.getSamples(1000);

for b = 1:length(bufferIn)
   fprintf("%d\n",b); 
end


clear m2k
