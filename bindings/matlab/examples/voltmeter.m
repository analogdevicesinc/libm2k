%{
This example reads the analog voltage from channel 0 of the analog input
%}
%% Setup
m2k = clib.libm2k.libm2k.context.m2kOpen();

if clibIsNull(m2k)
    clib.libm2k.libm2k.context.contextCloseAll();
    m2k = context.m2kOpen();
end
if isempty(m2k)
    error('M2K device not found');
end

%% Setup analog in
ain = m2k.getAnalogIn();
ain.reset();

ain.enableChannel(0,true);
voltage = ain.getVoltage(0);
disp(voltage);

%% Get more data
d3 = ain.getSamplesInterleaved_matlab(1024);
plot(d3)

clib.libm2k.libm2k.context.contextCloseAll();
clear m2k
