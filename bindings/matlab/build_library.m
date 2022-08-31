clear all; clc;

%% MATLAB API Builder
% This script will build the basic template file for the MATLAB bindings
% however since the library processor isn't perfect manual modifications
% need to be done with the generated interface file

%%
% Full path to files in the library
if isunix
hppPath = fullfile('/usr','local','include','libm2k');
libs = fullfile('/usr','local','lib','libm2k.so');
myPkg = 'libm2k';
else
error('Unix builds only tested so far');
end

%% Add related headers
h = {};

h1 = fullfile(hppPath,'m2kcalibration.hpp'); h = [{h1},h(:)'];
h1 = fullfile(hppPath,'m2kexceptions.hpp'); h = [{h1},h(:)'];
h1 = fullfile(hppPath,'logger.hpp'); h = [{h1},h(:)'];
h1 = fullfile(hppPath,'digital','genericdigital.hpp'); h = [{h1},h(:)'];
h1 = fullfile(hppPath,'digital','m2kdigital.hpp'); h = [{h1},h(:)'];
h1 = fullfile(hppPath,'analog','m2kanalogout.hpp'); h = [{h1},h(:)'];
h1 = fullfile(hppPath,'analog','m2kanalogin.hpp'); h = [{h1},h(:)'];
h1 = fullfile(hppPath,'m2kcalibration.hpp'); h = [{h1},h(:)'];
h1 = fullfile(hppPath,'analog','m2khardwaretrigger.hpp'); h = [{h1},h(:)'];
h1 = fullfile(hppPath,'analog','m2kpowersupply.hpp'); h = [{h1},h(:)'];
h1 = fullfile(hppPath,'analog','powersupply.hpp'); h = [{h1},h(:)'];
h1 = fullfile(hppPath,'utils','utils.hpp'); h = [{h1},h(:)'];
h1 = fullfile(hppPath,'utils','enums.hpp'); h = [{h1},h(:)'];
h1 = fullfile(hppPath,'contextbuilder.hpp'); h = [{h1},h(:)'];
h1 = fullfile(hppPath,'m2k.hpp'); h = [{h1},h(:)'];
h1 = fullfile(hppPath,'context.hpp'); h = [{h1},h(:)'];
h1 = fullfile(hppPath,'m2kglobal.hpp'); h = [{h1},h(:)'];
h1 = fullfile(hppPath,'digital','enums.hpp'); h = [{h1},h(:)'];
h1 = fullfile(hppPath,'analog','enums.hpp'); h = [{h1},h(:)'];
h1 = fullfile(hppPath,'enums.hpp'); h = [{h1},h(:)'];
h1 = fullfile('/usr','include','iio.h');  h = [{h1},h(:)'];
headers = h;

%% Build interface file
clibgen.generateLibraryDefinition(headers,...
    'IncludePath', hppPath,...
    'Libraries', libs,...
    'PackageName', myPkg,...
    'Verbose',true)
delete definelibm2k.mlx

%% Build library once manually updated
% pkg = definelibm2k;
% build(pkg);












