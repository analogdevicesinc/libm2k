% this test requires connection
%1+ to V+
%1- to GND
classdef libm2kPowerSupplyTest < matlab.unittest.TestCase

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
        function testPowerSupply(~)
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

            pushedValue = 1.7;
            ps.enableChannel(0,true);
            ps.pushChannel(0,pushedValue);

            errorMargin = 0.2;

            ain.enableChannel(0,true)
            readValue = ain.getVoltage(0);

            if((pushedValue - errorMargin) > readValue || readValue > (pushedValue + errorMargin))
                error('power supply test failed ');
            end

            context.contextCloseAll();

            clear m2k

        end

    end

end