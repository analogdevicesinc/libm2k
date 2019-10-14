%% Setup
import clib.libm2k.libm2k.*

m2k = devices.m2kOpen();

dig = m2k.getDigital();

dig.setSampleRateIn(100000);
dig.setSampleRateOut(100000);

N_BITS = 4;
DIO_OUTPUT = digital.DIO_DIRECTION.DIO_OUTPUT;

for k=1:N_BITS
    dig.setDirection(k-1,DIO_OUTPUT);
    dig.enableChannel(k-1,true);
end

out = uint16(2.^(0:N_BITS-1));

dig.setCyclic(true);
dig.push(out)

bufferIn = dig.getSamplesP(100);

bits = de2bi(bufferIn);

disp(bits(:,1:N_BITS));

clear m2k
