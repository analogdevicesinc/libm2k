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

classdef libm2kTests < matlab.unittest.TestCase

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
        
        function checkConnectivity(~)
            import clib.libm2k.libm2k.*
            m2k = context.m2kOpen();
            if isempty(m2k)
                error('No M2K found');
            end
        end
        function testDigital(~)
            import clib.libm2k.libm2k.*
            m2k = context.m2kOpen();
            if clibIsNull(m2k)
                clib.libm2k.libm2k.context.contextCloseAll();
                m2k = context.m2kOpen();
            end
            if isempty(m2k)
                error('M2K device not found');
            end
            if isempty(m2k)
                error('No M2K found');
            end
            dig = m2k.getDigital();
            if isempty(dig)
                error('No M2K digital found');
            end
            dig.setSampleRateIn(100000);
            dig.setSampleRateOut(100000);
        end
    end
    
end
