%% Setup
import clib.libm2k.libm2k.*
m2k = devices.m2kOpen();

%% Calibrate
m2k.calibrateADC();
m2k.calibrateDAC();

%% Set up devices
ain = m2k.getAnalogIn();
aout = m2k.getAnalogOut();
trig = ain.getTrigger();

%% Enables analog channels
ain.enableChannel(0,true);
ain.enableChannel(1,true);
ain.setSampleRate(100000);

c1 = analog.ANALOG_IN_CHANNEL.ANALOG_IN_CHANNEL_1;
ain.setRange(c1,-10,10)

%% Set up analog trigger
as = analog.M2K_TRIGGER_SOURCE.CHANNEL_1;
trig.setAnalogSource(as)

tc = analog.M2K_TRIGGER_CONDITION.RISING_EDGE;
trig.setAnalogCondition(0,tc);
trig.setAnalogLevel(0,0.5)
trig.setAnalogDelay(0)
tc = analog.M2K_TRIGGER_MODE.ANALOG;
trig.setAnalogMode(1,tc)

%% Set up analog output
aout.setSampleRate(0, 750000);
aout.setSampleRate(1, 750000);
aout.enableChannel(0, true)
aout.enableChannel(1, true)

x = linspace(-pi, pi, 1024);
b1 = sin(x);
b2 = linspace(-2,2,1024);
buffer = zeros(1,2048);
buffer(1:2:end) = b1;
buffer(2:2:end) = b2;

aout.setCyclic(true)
aout.pushInterleaved(buffer,2)

%% Collect analog data
for k=1:10
    data = ain.getSamplesInterleaved(1000);
    subplot(2,1,1);plot(data(1:2:end));
    subplot(2,1,2);plot(data(2:2:end));
    
    pause(0);
end

devices.deviceCloseAll();

clear m2k
