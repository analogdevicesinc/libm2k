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
