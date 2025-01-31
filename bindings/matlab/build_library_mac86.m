%
% Copyright (c) 2025 Analog Devices Inc.
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

clear all; %clc;

%% MATLAB API Builder
% This script will build the basic template file for the MATLAB bindings
% however since the library processor isn't perfect manual modifications
% need to be done with the generated interface file
mex -setup C++
%%
% Full path to files in the library
if ismac
    includepath = fullfile(pwd,'libm2k','include');
    hppPath = fullfile(pwd,'libm2k','include','libm2k');
    libs = fullfile(pwd,'libm2k','libm2k.dylib');
    myPkg = 'libm2k';
else
    error('Mac builds only tested so far');
end

%% Add related headers
h = {};

% 
h1 = fullfile(hppPath,'analog','m2kanalogout.hpp'); h = [{h1},h(:)'];
h1 = fullfile(hppPath,'analog','m2kanalogin.hpp'); h = [{h1},h(:)'];
h1 = fullfile(hppPath,'analog','m2kpowersupply.hpp'); h = [{h1},h(:)'];
h1 = fullfile(hppPath,'analog','dmm.hpp'); h = [{h1},h(:)'];
h1 = fullfile(hppPath,'analog','enums.hpp'); h = [{h1},h(:)'];

h1 = fullfile(hppPath,'digital','m2kdigital.hpp'); h = [{h1},h(:)'];
h1 = fullfile(hppPath,'digital','enums.hpp'); h = [{h1},h(:)'];

h1 = fullfile(hppPath,'utils','utils.hpp'); h = [{h1},h(:)'];
h1 = fullfile(hppPath,'utils','enums.hpp'); h = [{h1},h(:)'];

h1 = fullfile(hppPath,'m2khardwaretrigger.hpp'); h = [{h1},h(:)'];
h1 = fullfile(hppPath,'contextbuilder.hpp'); h = [{h1},h(:)'];
h1 = fullfile(hppPath,'m2k.hpp'); h = [{h1},h(:)'];
h1 = fullfile(hppPath,'m2kglobal.hpp'); h = [{h1},h(:)'];
h1 = fullfile(hppPath,'context.hpp'); h = [{h1},h(:)'];
h1 = fullfile(hppPath,'logger.hpp'); h = [{h1},h(:)'];
h1 = fullfile(hppPath,'m2kcalibration.hpp'); h = [{h1},h(:)'];
h1 = fullfile(hppPath,'m2kexceptions.hpp'); h = [{h1},h(:)'];
h1 = fullfile(hppPath,'enums.hpp'); h = [{h1},h(:)'];
% h1 = fullfile(pwd,'libm2k','include','iio.h');  h = [{h1},h(:)'];
headers = h;

% this section should be used when API changes apear and the define file
% needs to be regenerated
% Once the file is generated user needs to manualy uncomment libm2k
% functions they need
%% Build interface file
%% Add 'DefinedMacros' to fix builds using Visual Studio 16 2019
% delete definelibm2k.m
% clibgen.generateLibraryDefinition(headers,...
%     'IncludePath', includepath,...
%     'Libraries', libs,...
%     'InterfaceName', myPkg,...
%     'Verbose',true,...
%     'DefinedMacros', ["_HAS_CONDITIONAL_EXPLICIT=0", "_USE_EXTENDED_LOCALES_"])
% delete definelibm2k.mlx

%% Build library once manually updated
% pkg = definelibm2k_mac86;
% build(pkg);












