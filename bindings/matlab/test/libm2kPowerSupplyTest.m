%
% Copyright (c) 2024 Analog Devices Inc.
%
% This file is part of libm2k
% (see http://www.github.com/analogdevicesinc/libm2k).
%
% This program is free software; you can redistribute it and/or modify
% it under the terms of the GNU Lesser General Public License as published by
% the Free Software Foundation, either version 2.1 of the License, or
% (at your option) any later version.
%
% This program is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU Lesser General Public License for more details.
%
% You should have received a copy of the GNU Lesser General Public License
% along with this program. If not, see <http://www.gnu.org/licenses/>.
%

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