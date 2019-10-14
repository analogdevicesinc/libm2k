%% Setup
import clib.libm2k.libm2k.*
m2k = devices.m2kOpen();

%% Calibrate
m2k.calibrateADC();

ps = m2k.getPowerSupply();
ps.enableChannel(0,true);
ps.pushChannel(0,1.7);
ain = m2k.getAnalogIn();
ain.enableChannel(0,true)
disp(ain.getVoltage(0))

devices.deviceCloseAll();

clear m2k