%{
This example assumes the following connections:
W1 -> 1+
W2 -> 2+
GND -> 1-
GND -> 2-

The application will generate a sine and triangular wave on W1 and W2.
The signal is fed back into the analog input and the voltage values are
displayed on the screen
%}

trigger = false;

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
c2 = analog.ANALOG_IN_CHANNEL.ANALOG_IN_CHANNEL_2;
ain.setRange(c1,-10,10)
ain.setRange(c2,1,0)

%% Set up analog trigger
if trigger
    as = analog.M2K_TRIGGER_SOURCE.CHANNEL_1; %#ok<*UNRCH>
    trig.setAnalogSource(as)
    
    tc = analog.M2K_TRIGGER_CONDITION.RISING_EDGE;
    trig.setAnalogCondition(0,tc);
    trig.setAnalogLevel(0,0.5)
    trig.setAnalogDelay(0)
    tc = analog.M2K_TRIGGER_MODE.ANALOG;
    trig.setAnalogMode(0,tc)
end

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
% aout.push(0,b2)
% aout.push(1,b2)

%% Collect analog data
for k=1:10
    data = ain.getSamplesInterleaved_matlab(1000);
    data = data.double;
    subplot(2,1,1);plot(data(1:2:end));
    subplot(2,1,2);plot(data(2:2:end));
    
    pause(0);
end

clib.libm2k.libm2k.context.contextCloseAll();

clear m2k
