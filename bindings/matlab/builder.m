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

%% Top level library builder
if isfile('definelibm2k.m')
    delete definelibm2k.m
end

% Generate XML data cache
if ispc
    build_library_win64
    copyfile('definelibm2k_win64.m','definelibm2k.m');
else
    build_library_linux64
    copyfile('definelibm2k_linux64.m','definelibm2k.m');
end

% Build library
try
    pkg = definelibm2k;
    build(pkg);
catch
end

if ispc
    f = isfile(fullfile('libm2k','libm2kInterface.dll'));
else
    f = isfile(fullfile('libm2k','libm2kInterface.so'));
end

exit(~f);