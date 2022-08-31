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
            m2k = clib.libm2k.libm2k.context.m2kOpen();
            if isempty(m2k)
                error('No M2K found');
            end
        end
        function testDigital(~)
            m2k = clib.libm2k.libm2k.context.m2kOpen();
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
