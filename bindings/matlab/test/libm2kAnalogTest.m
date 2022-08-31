classdef libm2kAnalogTest < matlab.unittest.TestCase
    %             This example assumes the following connections:
    %              W1 -> 1+
    %              W2 -> 2+
    %              GND -> 1-
    %              GND -> 2-
    properties
        root = '';
    end

    methods(TestClassSetup)
        function addpaths(testCase)
            here = mfilename('fullpath');
            here = strsplit(here,'/');
            here = fullfile('/',here{1:end-2});
            testCase.root = here;
            addpath(genpath(fullfile(here,'libm2k')));
        end
    end

    methods(Test)
        function testAnalog(~)
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

            %% Set up devices
            ain = m2k.getAnalogIn();
            aout = m2k.getAnalogOut();

            %% Enables analog channels
            ain.enableChannel(0,true);
            ain.setSampleRate(10000);

            c1 = analog.ANALOG_IN_CHANNEL.ANALOG_IN_CHANNEL_1;
            ain.setRange(c1,-10,10)

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
            aout.setSampleRate(0, 7500);
            aout.enableChannel(0, true)

            x = linspace(-pi, pi, 1024);
            b1 = sin(x);
            buffer = b1;

            aout.setCyclic(true)
            aout.pushInterleaved(buffer,1)

            %% Collect analog data
            data = ain.getSamplesInterleaved_matlab(2048);
            data = data.double;
            R = corrcoef(buffer,data(1:2:end));
            if(R(1,2) < 0.7)
                error('analog test failed')
            end

            clib.libm2k.libm2k.context.contextCloseAll();
            clear m2k
        end

    end

end
