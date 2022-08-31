clear all; %clc;

%% MATLAB API Builder
% This script will build the basic template file for the MATLAB bindings
% however since the library processor isn't perfect manual modifications
% need to be done with the generated interface file

%%
% Full path to files in the library
if isunix
    includepath = fullfile(pwd,'libm2k','include');
    hppPath = fullfile(pwd,'libm2k','include','libm2k');
    libs = fullfile(pwd,'libm2k','libm2k.so');
    myPkg = 'libm2k';
else
    error('Linux builds only tested so far');
end

%% Add related headers
h = {};

% h1 = fullfile(hppPath,'m2kcalibration.hpp'); h = [{h1},h(:)'];
% h1 = fullfile(hppPath,'m2kexceptions.hpp'); h = [{h1},h(:)'];
% h1 = fullfile(hppPath,'logger.hpp'); h = [{h1},h(:)'];
% h1 = fullfile(hppPath,'digital','m2kdigital.hpp'); h = [{h1},h(:)'];
% h1 = fullfile(hppPath,'analog','m2kanalogout.hpp'); h = [{h1},h(:)'];
% h1 = fullfile(hppPath,'analog','m2kanalogin.hpp'); h = [{h1},h(:)'];
% h1 = fullfile(hppPath,'m2kcalibration.hpp'); h = [{h1},h(:)'];
% h1 = fullfile(hppPath,'m2khardwaretrigger.hpp'); h = [{h1},h(:)'];
% h1 = fullfile(hppPath,'analog','m2kpowersupply.hpp'); h = [{h1},h(:)'];
% h1 = fullfile(hppPath,'utils','utils.hpp'); h = [{h1},h(:)'];
% h1 = fullfile(hppPath,'utils','enums.hpp'); h = [{h1},h(:)'];
% h1 = fullfile(hppPath,'contextbuilder.hpp'); h = [{h1},h(:)'];
% h1 = fullfile(hppPath,'m2k.hpp'); h = [{h1},h(:)'];
% h1 = fullfile(hppPath,'m2kglobal.hpp'); h = [{h1},h(:)'];
% h1 = fullfile(hppPath,'context.hpp'); h = [{h1},h(:)'];
% h1 = fullfile(hppPath,'digital','enums.hpp'); h = [{h1},h(:)'];
% h1 = fullfile(hppPath,'analog','enums.hpp'); h = [{h1},h(:)'];
% h1 = fullfile(hppPath,'enums.hpp'); h = [{h1},h(:)'];
% % h1 = fullfile(pwd,'libm2k','include','iio.h');  h = [{h1},h(:)'];
% headers = h;

h1 = fullfile(hppPath,'digital','m2kdigital.hpp'); h = [{h1},h(:)'];
h1 = fullfile(hppPath,'digital','enums.hpp'); h = [{h1},h(:)'];

h1 = fullfile(hppPath,'analog','m2kanalogout.hpp'); h = [{h1},h(:)'];
h1 = fullfile(hppPath,'analog','m2kanalogin.hpp'); h = [{h1},h(:)'];
h1 = fullfile(hppPath,'analog','m2kpowersupply.hpp'); h = [{h1},h(:)'];
h1 = fullfile(hppPath,'analog','dmm.hpp'); h = [{h1},h(:)'];
h1 = fullfile(hppPath,'analog','enums.hpp'); h = [{h1},h(:)'];

% h1 = fullfile(hppPath,'utils','utils.hpp'); h = [{h1},h(:)'];
% h1 = fullfile(hppPath,'utils','enums.hpp'); h = [{h1},h(:)'];

%h1 = fullfile(hppPath,'m2kcalibration.hpp'); h = [{h1},h(:)'];
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

%% Build interface file
%clibgen.generateLibraryDefinition(headers,...
%    'IncludePath', includepath,...
%    'Libraries', libs,...
%    'PackageName', myPkg,...
%    'Verbose',true)
%delete definelibm2k.mlx

%% Build library once manually updated
pkg = definelibm2k;
build(pkg);












