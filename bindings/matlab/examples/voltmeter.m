%% Setup
m2k = clib.libm2k.libm2k.devices.m2kOpen();

%% Setup analog in
ain = m2k.getAnalogIn();
ain.enableChannel(0,true);
voltage = ain.getVoltage(0);
disp(voltage);

%% Get more data
d3 = ain.getSamplesInterleaved(1024);
plot(d3)

clear m2k
