%{
This exammple assumes connection:
1+ to V+
1- to GND
This example turns on the power supply and sets it to 1.7V and then reads 
back the value
%}
%% Setup
import clib.libm2k.libm2k.*
m2k = context.m2kOpen();

if clibIsNull(m2k)
    clib.libm2k.libm2k.context.contextCloseAll();
    m2k = context.m2kOpen();
end
if isempty(m2k)
    error('M2K device not found');
end

% Setup analog in
ain = m2k.getAnalogIn();
ain.reset();

% Will turn on the power supply if we need smth to measure
ps = m2k.getPowerSupply();

%% Calibrate
m2k.calibrateADC();
m2k.calibrateDAC();

ps.enableChannel(0,true);
ps.pushChannel(0,1.7);


ain.enableChannel(0,true)
disp(ain.getVoltage(0))

context.contextCloseAll();

clear m2k